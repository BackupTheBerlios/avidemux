#include <stdio.h>

#include <QApplication>
#include <QtCore>

static QTranslator qtTranslator;
static QTranslator avidemuxTranslator;

static void loadTranslation(QTranslator *qTranslator, QString translation)
{
	printf("[Locale] Loading language file %s ", translation.toUtf8().constData());	

	if (qTranslator->load(translation))
	{
		QApplication::installTranslator(qTranslator);
		printf("succeeded\n");
	}
	else
		printf("FAILED\n");		
}

void initTranslator(void) {}

const char* translate(const char *__domainname, const char *__msgid)
{
	static QMap<QString, QByteArray> map;
	QString msgid = QString(__msgid);

	if (!map.contains(msgid))
		map[msgid] = QApplication::translate("", __msgid).toUtf8();

	return map.value(msgid).constData();
}

#define HIDE_STRING_FROM_QT(domainname, msgid)  QApplication::translate(domainname, msgid) // to hide string from lupdate so a true test can be conducted

void loadTranslator(void)
{
	printf("\n[Locale] Locale: %s\n", QLocale::system().name().toUtf8().constData());

#ifdef __APPLE__
	QString appdir = QCoreApplication::applicationDirPath() + "/../Resources/locale/";
#else
	QString appdir = QCoreApplication::applicationDirPath() + "/i18n/";
#endif

	loadTranslation(&qtTranslator, appdir + "qt_" + QLocale::system().name());
	loadTranslation(&avidemuxTranslator, appdir + "avidemux_" + QLocale::system().name());

	printf("[Locale] Test: &Edit -> %s\n\n", HIDE_STRING_FROM_QT("MainWindow", "&Edit").toUtf8().data());
}

void destroyTranslator(void) {}

void getUIDescription(char* desc)
{
	sprintf(desc, "Qt4 (%s)", qVersion());
}
