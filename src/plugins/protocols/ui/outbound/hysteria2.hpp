#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_hysteria2.h"

class Hysteria2OutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::Hysteria2OutEditor
{
    Q_OBJECT

  public:
    explicit Hysteria2OutboundEditor(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e) override;
    virtual void Load() override {}
    void Store() override {}
};
