#include "documenthandler.h"

#include <QtGui/QTextDocument>
#include <QtGui/QTextCursor>
#include <QtGui/QFontDatabase>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>

#include "syntaxhighlighter.h"

DocumentHandler::DocumentHandler()
    : m_target(0)
    , m_doc(0)
    , m_highligher(0)
    , m_cursorPosition(-1)
    , m_selectionStart(0)
    , m_selectionEnd(0)
{
}

void DocumentHandler::setTarget(QQuickItem* target)
{
    m_doc = 0;
    m_target = target;
    if (!m_target)
        return;

    QVariant doc = m_target->property("textDocument");
    if (doc.canConvert<QQuickTextDocument*>()) {
        QQuickTextDocument* qqdoc = doc.value<QQuickTextDocument*>();
        if (qqdoc)
        {
            m_doc = qqdoc->textDocument();

            if (!m_highligher)
                m_highligher = new SyntaxHighlighter(m_doc);
            else
                m_highligher->setDocument(m_doc);
        }
    }
    emit targetChanged();
}

void DocumentHandler::setFileUrl(const QUrl& arg)
{
    if (m_fileUrl != arg) {
        m_fileUrl = arg;
        QString fileName = QQmlFile::urlToLocalFileOrQrc(arg);
        if (QFile::exists(fileName)) {
            QFile file(fileName);
            if (file.open(QFile::ReadOnly)) {
                QByteArray data = file.readAll();
                QTextCodec* codec = QTextCodec::codecForHtml(data);
                setText(codec->toUnicode(data));
                if (m_doc)
                    m_doc->setModified(false);
                if (fileName.isEmpty())
                    m_documentTitle = QStringLiteral("untitled.txt");
                else
                    m_documentTitle = QFileInfo(fileName).fileName();

                emit textChanged();
                emit documentTitleChanged();

                reset();
            }
        }
        emit fileUrlChanged();
    }
}

QString DocumentHandler::documentTitle() const
{
    return m_documentTitle;
}

void DocumentHandler::setDocumentTitle(QString arg)
{
    if (m_documentTitle != arg) {
        m_documentTitle = arg;
        emit documentTitleChanged();
    }
}

void DocumentHandler::setText(const QString& arg)
{
    if (m_text != arg) {
        m_text = arg;
        emit textChanged();
    }
}

void DocumentHandler::saveAs(const QUrl& arg, const QString& fileType)
{
    bool isHtml = fileType.contains(QLatin1String("htm"));
    QLatin1String ext(isHtml ? ".html" : ".txt");
    QString localPath = arg.toLocalFile();
    if (!localPath.endsWith(ext))
        localPath += ext;
    QFile f(localPath);
    if (!f.open(QFile::WriteOnly | QFile::Truncate | (isHtml ? QFile::NotOpen : QFile::Text))) {
        emit error(tr("Cannot save: ") + f.errorString());
        return;
    }
    f.write((isHtml ? m_doc->toHtml() : m_doc->toPlainText()).toLocal8Bit());
    f.close();
    qDebug() << "saved to" << localPath;
    setFileUrl(QUrl::fromLocalFile(localPath));
}

QUrl DocumentHandler::fileUrl() const
{
    return m_fileUrl;
}

QString DocumentHandler::text() const
{
    return m_text;
}

void DocumentHandler::setCursorPosition(int position)
{
    if (position == m_cursorPosition)
        return;

    m_cursorPosition = position;

    reset();
}

void DocumentHandler::reset()
{

}

QTextCursor DocumentHandler::textCursor() const
{
    QTextCursor cursor = QTextCursor(m_doc);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

void DocumentHandler::setSelectionStart(int position)
{
    m_selectionStart = position;
}

void DocumentHandler::setSelectionEnd(int position)
{
    m_selectionEnd = position;
}
