#include "vmess.hpp"

VmessOutboundEditor::VmessOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void VmessOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void VmessOutboundEditor::Load()
{
    vmess.loadJson(settings);
    securityCombo->setCurrentText(vmess.security);
    idLineEdit->setText(vmess.id);
    experimentsTxt->setText(vmess.experiments);
}

void VmessOutboundEditor::on_securityCombo_currentTextChanged(const QString &arg1)
{
    vmess.security = arg1;
}

void VmessOutboundEditor::on_idLineEdit_textChanged(const QString &arg1)
{
    vmess.id = arg1;
}

void VmessOutboundEditor::on_experimentsTxt_textChanged(const QString &arg1)
{
    vmess.experiments = arg1;
}
