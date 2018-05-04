#include "CurveModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "CurveView.h"
#include "curveeditorwidget.h"

#include "CurveSceneModule.h"

#include "Editor.h"
#include "proxies/MusicProxy.h"

#include <QWidget>

using namespace Idogep;
using namespace Grafkit;

Roles::ManageCurveAudiogramRole::ManageCurveAudiogramRole() : m_audiogramSampleCount(0), m_audiogramBuffer(nullptr)
{
}

Roles::ManageCurveAudiogramRole::~ManageCurveAudiogramRole()
{
	delete m_audiogramBuffer;
}


void Roles::ManageCurveAudiogramRole::GetAudiogram(QImage ** image, float startTime, float endTime, int rectWidth, int rectHeight)
{
	*image = nullptr;

	// todo: update when audiogramChanged
	if (!m_audiogramBuffer) {
		onRequestWaveform(m_audiogramBuffer, m_audiogramSampleCount, m_audiogramChannelCount, m_audiogramSamplePerSec);
	}

	uint32_t offset = uint32_t(startTime * m_audiogramSamplePerSec) * m_audiogramChannelCount;
	uint32_t length = uint32_t((endTime - startTime) * m_audiogramSamplePerSec) * m_audiogramChannelCount;

	float* buffer = &m_audiogramBuffer[offset];

	QImage* img = new QImage(QSize(rectWidth, rectHeight), QImage::Format_ARGB32);
	img->fill(0xFF303030);

	uint32_t readFrames = m_audiogramSampleCount;
	uint32_t valueHop = (readFrames / rectWidth) + ((readFrames % rectWidth) % 2);

	float invChannelCount;

	uint32_t fi = 0;
	for (uint32_t i = 0; i < readFrames * 2; i += valueHop * 2)
	{
		if (fi >= rectWidth) break;
		float vv = 0.0f;
		for (uint32_t k = 0; k < valueHop * 2; k += 2)
		{
			float sum = 0.0f;
			if ((i + (k / 2) + 1) >= length)
			{
				sum = 0.;
			}
			else {
				float * p = &buffer[i + (k / 2)];
				for (uint32_t l = 0; l < m_audiogramChannelCount; l++) {
					sum += fabs(p[l]);
				}
			}
		}

		vv /= float(valueHop) / invChannelCount;

		for (uint32_t j = 0; j < rectHeight; j++)
		{
			float cv = fabs((float(j)*2.0f) / float(rectHeight - 1) - 1.0f);
			if (vv >= cv) img->setPixel(fi, j, 0xFF3c3c3c);
		}

		fi++;
	}
	*image = img;
}

void Roles::ManageCurveAudiogramRole::ClearAudiogram()
{
	delete m_audiogramBuffer;
	m_audiogramBuffer = nullptr;
}


// ===========================================================================================

Idogep::CurveEditorModule::CurveEditorModule(Ref<Module> parent) : Module(parent), m_manageAudiogram(nullptr)
{
	m_curveScene = new CurveSceneModule(this);
	m_manageAudiogram = new Roles::ManageCurveAudiogramRole();
}

Idogep::CurveEditorModule::~CurveEditorModule()
{
	delete m_manageAudiogram;
}

void Idogep::CurveEditorModule::Initialize()
{
	assert(m_parent);
	assert(m_parent->GetView());

	QWidget* parentWidget = dynamic_cast<QWidget *>(m_parent->GetView().Get());
	assert(parentWidget);
	m_myView = new CurveEditorWidget(parentWidget);

	// manage playback role 
	Editor* editor = dynamic_cast<Editor*>(GetRootModule().Get());
	assert(editor);

	Timer* timer = editor->GetMusicProxy();
	assert(timer);

	m_myView->onTogglePlayback += Delegate(timer, &Timer::TogglePlay);

	// ... 
	Audiogram* music = editor->GetMusicProxy();
	assert(music);
	m_manageAudiogram->onRequestWaveform += Delegate(music, &Audiogram::GetWaveform);

	SetView(m_myView);
}
