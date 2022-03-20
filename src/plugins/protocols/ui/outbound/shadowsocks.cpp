#include "shadowsocks.hpp"

ShadowsocksOutboundEditor::ShadowsocksOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void ShadowsocksOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void ShadowsocksOutboundEditor::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    shadowsocks.password = arg1;
}

void ShadowsocksOutboundEditor::on_ss_encryptionMethod_currentTextChanged(const QString &arg1)
{
    shadowsocks.method = arg1;
}
