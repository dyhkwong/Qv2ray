#include "ProxyConfigurator.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "QvPlugin/Handlers/EventHandler.hpp"

#ifdef Q_OS_WIN
#include <windows.h>
#include <wininet.h>
#include <ras.h>
#include <raserror.h>
#endif // Q_OS_WIN

namespace Qv2ray::components::proxy
{
    using ProcessArgument = QPair<QString, QStringList>;

#ifdef Q_OS_MACOS
    QStringList getNetworkServices()
    {
        QProcess p;
        p.setProgram("/usr/sbin/networksetup");
        p.setArguments(QStringList{ "-listallnetworkservices" });
        p.start();
        p.waitForStarted();
        p.waitForFinished();
        qInfo() << p.errorString();
        auto lines = SplitLines(p.readAllStandardOutput());
        QStringList list;
        // Start from 1 since the first line is unneeded.
        for (auto i = 1; i < lines.count(); i++)
        {
            // An asterisk (*) denotes that a network service is disabled.
            if (!lines[i].startsWith("*"))
            {
                list << lines[i];
            }
        }
        qInfo() << "Found " << QString::number(list.size()) << " network service(s): " << list.join(";");
        return list;
    }
#endif // Q_OS_MACOS

#ifdef Q_OS_WIN
    bool setProxyOptions(LPWSTR addrW)
    {
        INTERNET_PER_CONN_OPTION_LIST list;
        DWORD dwBufSize = sizeof(list);
        list.dwSize = dwBufSize;
        // NULL == LAN, otherwise connectoid name.
        list.pszConnection = nullptr;
        // Clear system proxy if proxy address is NULL, otherwise set system proxy.
        if (nullptr == addrW)
        {
            list.dwOptionCount = 1;
            list.pOptions = new INTERNET_PER_CONN_OPTION[1];
            if (nullptr == list.pOptions)
            {
                return false;
            }
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT;
        }
        else
        {
            list.dwOptionCount = 2;
            list.pOptions = new INTERNET_PER_CONN_OPTION[2];
            if (nullptr == list.pOptions)
            {
                return false;
            }
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_PROXY;
            list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
            list.pOptions[1].Value.pszValue = addrW;
        }
        // Set proxy for LAN.
        if (!InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize))
        {
            qInfo() << "InternetSetOption failed for LAN, GLE=" << QString::number(GetLastError());
        }
        DWORD dwCb = 0;
        DWORD dwRet = ERROR_SUCCESS;
        DWORD dwEntries = 0;
        LPRASENTRYNAME lpRasEntryName = nullptr;
        // Call RasEnumEntries with lpRasEntryName = NULL. dwCb is returned with the required buffer size and
        // a return code of ERROR_BUFFER_TOO_SMALL
        dwRet = RasEnumEntries(nullptr, nullptr, lpRasEntryName, &dwCb, &dwEntries);
        if (ERROR_BUFFER_TOO_SMALL == dwRet)
        {
            lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
            if (nullptr == lpRasEntryName)
            {
                qInfo() << "HeapAlloc failed";
                delete[] list.pOptions;
                return false;
            }
            // The first RASENTRYNAME structure in the array must contain the structure size.
            lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);
            // Call RasEnumEntries to enumerate all RAS entry names.
            if (ERROR_SUCCESS != RasEnumEntries(nullptr, nullptr, lpRasEntryName, &dwCb, &dwEntries))
            {
                qInfo() << ("RasEnumEntries failed");
                HeapFree(GetProcessHeap(), 0, lpRasEntryName);
                lpRasEntryName = nullptr;
                delete[] list.pOptions;
                return false;
            }
            // Set proxy for each connectoid.
            for (DWORD i = 0; i < dwEntries; ++i)
            {
                list.pszConnection = lpRasEntryName[i].szEntryName;
                if (!InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize))
                {
                    qInfo() << "InternetSetOption failed for connectoid: " << QString::fromWCharArray(list.pszConnection) << ", GLE=" << QString::number(GetLastError());
                }
            }
            HeapFree(GetProcessHeap(), 0, lpRasEntryName);
            lpRasEntryName = nullptr;
        }
        delete[] list.pOptions;
        InternetSetOption(nullptr, INTERNET_OPTION_SETTINGS_CHANGED, nullptr, 0);
        InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
        return true;
    }
#endif // Q_OS_WIN

    void SetSystemProxy(const QString &httpAddress, const QString &socksAddress, int httpPort, int socksPort)
    {
        bool hasHTTP = !httpAddress.isEmpty();
        bool hasSOCKS = !socksAddress.isEmpty();
#ifdef Q_OS_WIN
        if (!hasHTTP)
        {
            qInfo() << "HTTP proxy not found. Qv2ray will not set system proxy.";
            return;
        }
        else
        {
            qInfo() << "Qv2ray will set system proxy to use HTTP.";
        }
#else
        if (!hasHTTP && !hasSOCKS)
        {
            qInfo() << "HTTP or SOCKS proxy not found. Qv2ray will not set system proxy.";
            return;
        }
        if (hasHTTP)
        {
            qInfo() << "Qv2ray will set system proxy to use HTTP.";
        }
        if (hasSOCKS)
        {
            qInfo() << "Qv2ray will set system proxy to use SOCKS.";
        }
#endif
        qInfo() << "Setting up System Proxy...";

#ifdef Q_OS_WIN
        QString addr;
        const QHostAddress ha(httpAddress);
        if (ha.protocol() == QAbstractSocket::IPv6Protocol)
        {
            addr = "[" + ha.toString() + "]:" + QString::number(httpPort);
        }
        else
        {
            addr = httpAddress + ":" + QString::number(httpPort);
        }
        auto addrW = new WCHAR[addr.length() + 1];
        wcscpy(addrW, addr.toStdWString().c_str());
        if (!setProxyOptions(addrW))
        {
            qInfo() << "Failed to set system proxy.";
        }
#elif defined(Q_OS_LINUX)
        QList<ProcessArgument> actions;
        actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy", "mode", "manual" } };
        const auto kdeVersion = qEnvironmentVariableIntValue("KDE_SESSION_VERSION");
        const auto isKDE = qEnvironmentVariable("XDG_CURRENT_DESKTOP") == "KDE" && kdeVersion >= 5;
        const auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        // Configure HTTP Proxy for HTTP, FTP and HTTPS
        if (hasHTTP)
        {
            for (const auto &protocol : QStringList{ "http", "ftp", "https" })
            {
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy." + protocol, "host", httpAddress } };
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy." + protocol, "port", QString::number(httpPort) } };
                if (isKDE)
                {
                    actions << ProcessArgument{ "kwriteconfig" + QString::number(kdeVersion),
                                                { "--file", configPath + "/kioslaverc", //
                                                  "--group", "Proxy Settings",          //
                                                  "--key", protocol + "Proxy",          //
                                                  "http://" + httpAddress + " " + QString::number(httpPort) } };
                }
            }
        }
        // Configure SOCKS5 Proxy
        if (hasSOCKS)
        {
            actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy.socks", "host", socksAddress } };
            actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy.socks", "port", QString::number(socksPort) } };
            if (isKDE)
            {
                actions << ProcessArgument{ "kwriteconfig" + QString::number(kdeVersion),
                                            { "--file", configPath + "/kioslaverc", //
                                              "--group", "Proxy Settings",          //
                                              "--key", "socksProxy",                //
                                              "socks://" + socksAddress + " " + QString::number(socksPort) } };
            }
        }
        // Set Proxy Mode to Manual
        actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy", "mode", "manual" } };
        if (isKDE)
        {
            actions << ProcessArgument{ "kwriteconfig" + QString::number(kdeVersion), { "--file", configPath + "/kioslaverc", "--group", "Proxy Settings", "--key", "ProxyType", "1" } };
            // Notify kioslaves to reload system proxy configuration.
            actions << ProcessArgument{ "dbus-send", { "--type=signal", "/KIO/Scheduler", "org.kde.KIO.Scheduler.reparseSlaveConfiguration", "string:''" } };
        }
        // Execute them all!
        // note: do not use std::all_of / any_of / none_of,
        // because those are short-circuit and cannot guarantee atomicity.
        QList<bool> results;
        for (const auto &action : actions)
        {
            // execute and get the return code
            const auto returnCode = QProcess::execute(action.first, action.second);
            // print out the commands and return codes
            qInfo() << action.first << action.second.join(';') << returnCode;
            // give the return code back
            results << (returnCode == QProcess::NormalExit);
        }
        if (results.count(true) != actions.size())
        {
            qInfo() << "Something wrong when setting system proxy.";
        }
#elif defined(Q_OS_MACOS)
        for (const auto &service : getNetworkServices())
        {
            if (hasHTTP)
            {
                QProcess::execute("/usr/sbin/networksetup", { "-setwebproxy", service, httpAddress, QString::number(httpPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxy", service, httpAddress, QString::number(httpPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setwebproxystate", service, "on" });
                QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxystate", service, "on" });
            }
            if (hasSOCKS)
            {
                QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxy", service, socksAddress, QString::number(socksPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxystate", service, "on" });
            }
        }
#endif
    }

    void ClearSystemProxy()
    {
        qInfo() << "Clearing System Proxy...";
#ifdef Q_OS_WIN
        if (!setProxyOptions(nullptr))
        {
            qInfo() << "Failed to clear system proxy.";
        }
#elif defined(Q_OS_LINUX)
        QList<ProcessArgument> actions;
        const auto kdeVersion = qEnvironmentVariableIntValue("KDE_SESSION_VERSION");
        bool isKDE = qEnvironmentVariable("XDG_CURRENT_DESKTOP") == "KDE" && kdeVersion >= 5;
        const auto configRoot = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        // Setting System Proxy Mode to None
        {
            actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy", "mode", "none" } };
            if (isKDE)
            {
                actions << ProcessArgument{ "kwriteconfig" + QString::number(kdeVersion),
                                            { "--file", configRoot + "/kioslaverc", //
                                              "--group", "Proxy Settings",          //
                                              "--key", "ProxyType", "0" } };
                // Notify kioslaves to reload system proxy configuration.
                actions << ProcessArgument{ "dbus-send",
                                            { "--type=signal", "/KIO/Scheduler",                 //
                                              "org.kde.KIO.Scheduler.reparseSlaveConfiguration", //
                                              "string:''" } };
            }
        }
        // Execute the Actions.
        for (const auto &action : actions)
        {
            // execute and get the return code
            const auto returnCode = QProcess::execute(action.first, action.second);
            // print out the commands and result codes
            qInfo() << action.first << action.second.join(";") << returnCode;
        }
#elif defined(Q_OS_MACOS)
        for (const auto &service : getNetworkServices())
        {
            QProcess::execute("/usr/sbin/networksetup", { "-setautoproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setwebproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxystate", service, "off" });
        }
#endif
    }
} // namespace Qv2ray::components::proxy
