#include "CurveEditorView.h"

#include<QDebug>

using namespace Idogep;

CurveEditorView::CurveEditorView() : View()
    , m_isDisplayWaveform(true)
    , m_isDisplayCurve(true) 
{
    // ...
}

void CurveEditorView::Invalidate() {
    qWarning() << "CurveEditorView::Invalidate NOT IMPLEMENTED";
}
