#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_trojan.h"

class TrojanOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::TrojanOutEditor
{
    Q_OBJECT

  public:
    explicit TrojanOutboundEditor(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e) override;
    virtual void Load() override
    {
        trojan.loadJson(settings);
        passwdTxt->setText(trojan.password);
    }
    void Store() override
    {
        settings = IOProtocolSettings{ trojan.toJson() };
    }

  private slots:
    void on_passwdTxt_textEdited(const QString &arg1);

  private:
    Qv2ray::Models::TrojanClientObject trojan;
};
