#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_shadowsocks.h"

class ShadowsocksOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::shadowsocksOutEditor
{
    Q_OBJECT

  public:
    explicit ShadowsocksOutboundEditor(QWidget *parent = nullptr);

    void Load() override
    {
        shadowsocks.loadJson(settings);
        ss_passwordTxt->setText(shadowsocks.password);
        ss_encryptionMethod->setCurrentText(shadowsocks.method);
    }

    void Store() override
    {
        settings = IOProtocolSettings{ shadowsocks.toJson() };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_ss_encryptionMethod_currentTextChanged(const QString &arg1);
    void on_ss_passwordTxt_textEdited(const QString &arg1);

  private:
    Qv2ray::Models::ShadowSocksClientObject shadowsocks;
};
