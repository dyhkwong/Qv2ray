#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_httpout.h"

class HttpOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::httpOutEditor
{
    Q_OBJECT

  public:
    explicit HttpOutboundEditor(QWidget *parent = nullptr);

    void Load() override
    {
        http.loadJson(settings);
        http_UserNameTxt->setText(http.user);
        http_PasswordTxt->setText(http.pass);
    }

    void Store() override
    {
        settings = IOProtocolSettings{ http.toJson() };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_http_UserNameTxt_textEdited(const QString &arg1);
    void on_http_PasswordTxt_textEdited(const QString &arg1);

  private:
    Qv2ray::Models::HTTPSOCKSObject http;
};
