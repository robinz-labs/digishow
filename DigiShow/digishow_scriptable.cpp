/*
    Copyright 2024 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */

#include "digishow_scriptable.h"
#include "app_utilities.h"

DigishowScriptable::DigishowScriptable(QObject *parent)
    : QObject{parent}
{
    m_qmlEngine = new QQmlEngine();
    m_qmlComponent = nullptr;
    m_qmlObject = nullptr;
}

DigishowScriptable::~DigishowScriptable()
{
    if (m_qmlObject != nullptr) delete m_qmlObject;
    if (m_qmlComponent != nullptr) delete m_qmlComponent;
    delete m_qmlEngine;
}

bool DigishowScriptable::start(const QString &filepath)
{
    if (m_qmlComponent != nullptr || m_qmlObject != nullptr) stop();

    m_qmlComponent = new QQmlComponent(m_qmlEngine);

    if (!filepath.isEmpty() && AppUtilities::fileExists(filepath)) {
        // a customized script
        QString qml = AppUtilities::loadStringFromFile(filepath);
        QUrl url = QUrl("qrc:///DigishowScriptableCustomized.qml");
        m_qmlComponent->setData(qml.toUtf8(), url);
    } else {
        // the base script
        QUrl url = QUrl("qrc:///DigishowScriptable.qml");
        m_qmlComponent->loadUrl(url);
    }

    if (!m_qmlComponent->isReady()) return false;
    m_qmlObject = m_qmlComponent->create();

    QVariant r;
    QMetaObject::invokeMethod(
                m_qmlObject, "onStart", Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, r));

    return true;
}

void DigishowScriptable::stop()
{
    if (m_qmlObject != nullptr) {
        QVariant r;
        QMetaObject::invokeMethod(
                    m_qmlObject, "onStop", Qt::DirectConnection,
                    Q_RETURN_ARG(QVariant, r));

        delete m_qmlObject;
        m_qmlObject = nullptr;
    }

    if (m_qmlComponent != nullptr) {
        delete m_qmlComponent;
        m_qmlComponent = nullptr;
    }
}


QVariant DigishowScriptable::execute(const QString &script)
{
    if (m_qmlObject == nullptr) return QVariant();

    QVariant r;
    QMetaObject::invokeMethod(
                m_qmlObject, "execute", Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, r),
                Q_ARG(QVariant, script));
    return r;
}

int DigishowScriptable::execute(const QString &expression, int inputValue, bool *ok)
{
    if (ok != nullptr) *ok = false;
    if (m_qmlObject == nullptr) return 0;

    QVariant r;
    QMetaObject::invokeMethod(
                m_qmlObject, "execute", Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, r),
                Q_ARG(QVariant, expression),
                Q_ARG(QVariant, inputValue));

    if (r.isNull() || !r.isValid()) return 0;

    if (ok != nullptr) *ok = true;
    return r.toInt();
}

