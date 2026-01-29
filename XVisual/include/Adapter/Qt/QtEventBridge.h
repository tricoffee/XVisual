#ifndef XVISUAL_ADAPTER_QT_QTEVENTBRIDGE_H
#define XVISUAL_ADAPTER_QT_QTEVENTBRIDGE_H

#include <functional>

#include <QObject>
#include <QMetaObject>

#include "Core/Executor/IEventSink.h"

namespace XVisual {

/**
 * QtEventBridge marshals Core events from worker thread to Qt main thread.
 *
 * Usage:
 *  - Construct with a QObject that lives in the UI thread (e.g. MainWindow*).
 *  - Provide a handler that will run in UI thread.
 *
 * Note: This class itself does NOT inherit QObject, so it doesn't require moc.
 */
class QtEventBridge final : public IEventSink
{
public:
	using Handler = std::function<void(const NodeEvent&)>;

	QtEventBridge(QObject* uiThreadObject, Handler handler)
		: m_uiObject(uiThreadObject)
		, m_handler(std::move(handler))
	{
	}

	void onEvent(const NodeEvent& e) override
	{
		if (!m_uiObject || !m_handler)
			return;

		// Copy the event (it may outlive the caller stack).
		NodeEvent copy = e;
		QMetaObject::invokeMethod(m_uiObject,
			[handler = m_handler, ev = std::move(copy)]() mutable
			{
				handler(ev);
			},
			Qt::QueuedConnection);
	}

private:
	QObject* m_uiObject = nullptr;
	Handler m_handler;
};

} // namespace XVisual

#endif // XVISUAL_ADAPTER_QT_QTEVENTBRIDGE_H
