#pragma once

#include "MessageBus/MessageBus.hpp"
#include "plugins/PluginsCommon/V2RayModels.hpp"
#include "ui_RouteSettingsWidget.h"

class RouteSettingsWidget
    : public QWidget
    , private Ui::RouteSettingsWidget
{
    Q_OBJECT

  public:
    explicit RouteSettingsWidget(QWidget *parent = nullptr);
    void SetRoute(const Qv2ray::Models::V2RayRoutingObject &);
    Qv2ray::Models::V2RayRoutingObject GetRouteConfig();

  private slots:
    void on_addRuleBtn_clicked();
    void on_removeRuleBtn_clicked();
    void on_rulesListbox_currentRowChanged(int currentRow);
    void on_moveRuleUpBtn_clicked();
    void on_moveRuleDownBtn_clicked();
    void on_disabledCB_stateChanged(int arg1);
    void on_nameTxt_textEdited(const QString &arg1);
    void on_domainTxt_textEdited(const QString &arg1);
    void on_ipTxt_textEdited(const QString &arg1);
    void on_portTxt_textEdited(const QString &arg1);
    void on_srcPortTxt_textEdited(const QString &arg1);
    void on_networkCombo_currentIndexChanged(int arg1);
    void on_sourceIpTxt_textEdited(const QString &arg1);
    void on_protocolTxt_textEdited(const QString &arg1);
    void on_outboundCombo_currentIndexChanged(int arg1);

  private:
    void updateColorScheme();
    void ShowCurrentRuleDetails();
    QvMessageBusSlotDecl;
    Qv2ray::Models::V2RayRoutingObject route;
};
