#include "RouteSettingsWidget.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "ui/WidgetUIBase.hpp"

#define CHECK_DISABLE_MOVE_BTN              \
    if (rulesListbox->count() <= 1)         \
    {                                       \
        moveRuleUpBtn->setEnabled(false);   \
        moveRuleDownBtn->setEnabled(false); \
    }

#define UPDATE_UI_ENABLED_STATE                             \
    disabledCB->setEnabled(rulesListbox->count() > 0);      \
    nameTxt->setEnabled(rulesListbox->count() > 0);         \
    domainTxt->setEnabled(rulesListbox->count() > 0);       \
    ipTxt->setEnabled(rulesListbox->count() > 0);           \
    portTxt->setEnabled(rulesListbox->count() > 0);         \
    srcPortTxt->setEnabled(rulesListbox->count() > 0);      \
    removeRuleBtn->setEnabled(rulesListbox->count() > 0);   \
    networkCombo->setEnabled(rulesListbox->count() > 0);    \
    sourceIpTxt->setEnabled(rulesListbox->count() > 0);     \
    protocolTxt->setEnabled(rulesListbox->count() > 0);     \
    outboundCombo->setEnabled(rulesListbox->count() > 0);   \
    CHECK_DISABLE_MOVE_BTN

#define currentRuleIndex rulesListbox->currentRow()

void RouteSettingsWidget::updateColorScheme()
{
    addRuleBtn->setIcon(QIcon(STYLE_RESX("add")));
    removeRuleBtn->setIcon(QIcon(STYLE_RESX("minus")));
    moveRuleUpBtn->setIcon(QIcon(STYLE_RESX("arrow-up")));
    moveRuleDownBtn->setIcon(QIcon(STYLE_RESX("arrow-down")));
}

RouteSettingsWidget::RouteSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    QvMessageBusConnect();

    UPDATE_UI_ENABLED_STATE;
    updateColorScheme();
}

QvMessageBusSlotImpl(RouteSettingsWidget)
{
    switch (msg)
    {
        case MessageBus::UPDATE_COLORSCHEME:
        {
            updateColorScheme();
            break;
        }
    }
}

void RouteSettingsWidget::ShowCurrentRuleDetails()
{
    disabledCB->setChecked((*route.rules)[currentRuleIndex].disabled);
    nameTxt->setText((*route.rules)[currentRuleIndex].ruleName);
    domainTxt->setText((*route.rules)[currentRuleIndex].domains->join("|"));
    ipTxt->setText((*route.rules)[currentRuleIndex].ip->join("|"));
    portTxt->setText((*route.rules)[currentRuleIndex].port);
    srcPortTxt->setText((*route.rules)[currentRuleIndex].sourcePort);
    networkCombo->setCurrentIndex((*route.rules)[currentRuleIndex].network);
    sourceIpTxt->setText((*route.rules)[currentRuleIndex].source->join("|"));
    protocolTxt->setText((*route.rules)[currentRuleIndex].protocol->join("|"));
    outboundCombo->setCurrentIndex((*route.rules)[currentRuleIndex].outboundTag);
}

void RouteSettingsWidget::SetRoute(const Qv2ray::Models::V2RayRoutingObject &conf)
{
    this->route = conf;
    rulesListbox->clear();
    std::for_each(route.rules->begin(), route.rules->end(), [&](const auto &route) { rulesListbox->addItem(route.ruleName); });
    if (rulesListbox->count() > 0)
    {
        rulesListbox->setCurrentRow(0);
        ShowCurrentRuleDetails();
    }
    UPDATE_UI_ENABLED_STATE
}

Qv2ray::Models::V2RayRoutingObject RouteSettingsWidget::GetRouteConfig()
{
    return route;
}

void RouteSettingsWidget::on_addRuleBtn_clicked()
{
    Qv2ray::Models::V2RayRoutingObject::V2RayRoutingRuleObject o;
    o.ruleName = u"rule"_qs;
    o.network = V2RayRoutingObject::Network::TCPUDP;
    o.outboundTag = V2RayRoutingObject::OutboundTag::Proxy;
    route.rules->push_back(o);
    rulesListbox->addItem(o.ruleName);
    rulesListbox->setCurrentRow(rulesListbox->count() - 1);
    UPDATE_UI_ENABLED_STATE
    ShowCurrentRuleDetails();
}

void RouteSettingsWidget::on_removeRuleBtn_clicked()
{
    route.rules->removeAt(currentRuleIndex);
    // Block the signals
    rulesListbox->blockSignals(true);
    auto item = rulesListbox->item(currentRuleIndex);
    rulesListbox->removeItemWidget(item);
    delete item;
    rulesListbox->blockSignals(false);
    UPDATE_UI_ENABLED_STATE

    if (rulesListbox->count() > 0)
    {
        if (currentRuleIndex < 0)
            rulesListbox->setCurrentRow(0);
        ShowCurrentRuleDetails();
    }
}

void RouteSettingsWidget::on_rulesListbox_currentRowChanged(int currentRow)
{
    if (currentRow < 0)
        return;

    moveRuleUpBtn->setEnabled(true);
    moveRuleDownBtn->setEnabled(true);
    if (currentRow == 0)
    {
        moveRuleUpBtn->setEnabled(false);
    }
    if (currentRow == rulesListbox->count() - 1)
    {
        moveRuleDownBtn->setEnabled(false);
    }

    ShowCurrentRuleDetails();
}

void RouteSettingsWidget::on_moveRuleUpBtn_clicked()
{
    auto temp = (*route.rules)[currentRuleIndex - 1];
    (*route.rules)[currentRuleIndex - 1] = (*route.rules)[currentRuleIndex];
    (*route.rules)[currentRuleIndex] = temp;

    rulesListbox->currentItem()->setText((*route.rules)[currentRuleIndex].ruleName);
    rulesListbox->setCurrentRow(currentRuleIndex - 1);
    rulesListbox->currentItem()->setText((*route.rules)[currentRuleIndex].ruleName);
}

void RouteSettingsWidget::on_moveRuleDownBtn_clicked()
{
    auto temp = (*route.rules)[currentRuleIndex + 1];
    (*route.rules)[currentRuleIndex + 1] = (*route.rules)[currentRuleIndex];
    (*route.rules)[currentRuleIndex] = temp;

    rulesListbox->currentItem()->setText((*route.rules)[currentRuleIndex].ruleName);
    rulesListbox->setCurrentRow(currentRuleIndex + 1);
    rulesListbox->currentItem()->setText((*route.rules)[currentRuleIndex].ruleName);
}

void RouteSettingsWidget::on_disabledCB_stateChanged(int arg1)
{
    (*route.rules)[currentRuleIndex].disabled = arg1 == Qt::Checked;
}

void RouteSettingsWidget::on_nameTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].ruleName = arg1;
    rulesListbox->currentItem()->setText(arg1);
}

void RouteSettingsWidget::on_domainTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].domains = arg1.split("|");
}

void RouteSettingsWidget::on_ipTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].ip = arg1.split("|");
}

void RouteSettingsWidget::on_portTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].port = arg1;
}

void RouteSettingsWidget::on_srcPortTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].sourcePort = arg1;
}

void RouteSettingsWidget::on_sourceIpTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].source = arg1.split("|");
}

void RouteSettingsWidget::on_protocolTxt_textEdited(const QString &arg1)
{
    (*route.rules)[currentRuleIndex].protocol = arg1.split("|");
}

void RouteSettingsWidget::on_networkCombo_currentIndexChanged(int arg1)
{
    (*route.rules)[currentRuleIndex].outboundTag = static_cast<V2RayRoutingObject::OutboundTag>(arg1);
}

void RouteSettingsWidget::on_outboundCombo_currentIndexChanged(int arg1)
{
    (*route.rules)[currentRuleIndex].outboundTag = static_cast<V2RayRoutingObject::OutboundTag>(arg1);
}
