#pragma once
#include <QHostAddress>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QString>

namespace Qv2ray::components::proxy
{
    void ClearSystemProxy();
    void SetSystemProxy(const QString &http_address, const QString &socks_address, int http_port, int socks_port);
} // namespace Qv2ray::components::proxy

using namespace Qv2ray::components;
using namespace Qv2ray::components::proxy;
