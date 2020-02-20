#pragma once

#include <fx.h>

// We include here only the public interface elementary plot library header.
#include "elem_plot.h"

class window_fox;

namespace FX {

class FXElemBaseWindow : public FXWindow {
    FXDECLARE(FXElemBaseWindow)
protected:
    FXElemBaseWindow(): m_window(nullptr) { }
private:
    FXElemBaseWindow(const FXElemBaseWindow&);
    FXElemBaseWindow &operator=(const FXElemBaseWindow&);
public:
    FXElemBaseWindow(FXComposite* p, window_fox *win = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);
    ~FXElemBaseWindow();

    void setWindowFox(window_fox *win) {
        m_window = win;
    }

    void position(FXint x, FXint y, FXint w, FXint h) override;
    void create() override;

    long onUpdateRegion (FXObject *, FXSelector, void *);
    long onCloseRequest (FXObject *, FXSelector, void *);
    long onPaint        (FXObject *, FXSelector, void *);
    long onMap          (FXObject *, FXSelector, void *);

    enum {
        ID_ELEM_UPDATE = FXWindow::ID_LAST,
        ID_ELEM_CLOSE,
        ID_LAST
    };

private:
    window_fox *m_window;
};
}

enum FXElemCreatePolicy {
    ELEM_CREATE_DEFER = 0,
    ELEM_CREATE_NOW = 1,
};

struct FXElemStartMessage {
    window_fox *window;
    unsigned width;
    unsigned height;
    unsigned flags;
};

void FXElemBuildWindow(FXComposite *p, FXElemStartMessage *message, FXElemCreatePolicy create_flag);
