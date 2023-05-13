#pragma once

#include "MessageBus/MessageBus.hpp"
#include "plugins/PluginsCommon/V2RayModels.hpp"
#include "ui_DnsSettingsWidget.h"

namespace Qv2ray::ui::widgets
{
    class AutoCompleteTextEdit;
}

class DnsSettingsWidget
    : public QWidget
    , private Ui::DnsSettingsWidget
{
    Q_OBJECT

  public:
    explicit DnsSettingsWidget(QWidget *parent = nullptr);
    void SetDNSObject(const Qv2ray::Models::V2RayDNSObject &dns);
    Qv2ray::Models::V2RayDNSObject GetDNSObject();

  private slots:
    void on_dnsClientIPTxt_textEdited(const QString &arg1);
    void on_dnsTagTxt_textEdited(const QString &arg1);
    void on_addServerBtn_clicked();
    void on_removeServerBtn_clicked();
    void on_serversListbox_currentRowChanged(int currentRow);
    void on_moveServerUpBtn_clicked();
    void on_moveServerDownBtn_clicked();
    void on_serverAddressTxt_textEdited(const QString &arg1);
    void on_serverPortSB_valueChanged(int arg1);
    void on_addStaticHostBtn_clicked();
    void on_removeStaticHostBtn_clicked();
    void on_detailsSettingsGB_toggled(bool arg1);
    void on_staticResolvedDomainsTable_cellChanged(int row, int column);
    void on_dnsDomainMatcherCB_currentTextChanged(const QString &arg1);
    void on_dnsQueryStrategyCB_currentTextChanged(const QString &arg1);
    void on_dnsCacheStrategyCB_currentTextChanged(const QString &arg1);
    void on_dnsFallbackStrategyCB_currentTextChanged(const QString &arg1);
    void on_queryStrategyCB_currentTextChanged(const QString &arg1);
    void on_cacheStrategyCB_currentTextChanged(const QString &arg1);
    void on_fallbackStrategyCB_currentTextChanged(const QString &arg1);
    void on_fakeDNSCB_stateChanged(int arg1);

  private:
    void updateColorScheme();
    void ShowCurrentDnsServerDetails();
    void ProcessDnsPortEnabledState();
    QvMessageBusSlotDecl;
    Qv2ray::Models::V2RayDNSObject dns;
    // int currentServerIndex;
    //
    Qv2ray::ui::widgets::AutoCompleteTextEdit *domainListTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *ipListTxt;
};
