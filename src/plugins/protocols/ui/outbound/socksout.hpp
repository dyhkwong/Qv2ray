#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_socksout.h"

class SocksOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::socksOutEditor
{
    Q_OBJECT

  public:
    explicit SocksOutboundEditor(QWidget *parent = nullptr);

    virtual void Load() override
    {
        socks.loadJson(settings);
        socks_UserNameTxt->setText(socks.user);
        socks_PasswordTxt->setText(socks.pass);
        socks_VersionCB->setCurrentText(socks.version);
    }

    virtual void Store() override
    {
        settings = IOProtocolSettings{ socks.toJson() };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_socks_UserNameTxt_textEdited(const QString &arg1);
    void on_socks_PasswordTxt_textEdited(const QString &arg1);
    void on_socks_VersionCB_currentTextChanged(const QString &arg1);

  private:
    Qv2ray::Models::HTTPSOCKSObject socks;
};
