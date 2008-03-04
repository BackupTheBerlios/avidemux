#include <stdio.h>
#include <string.h>

#include <QApplication>
#include <QLocale>
#include <QTranslator>

static const int MAX_MESSAGE_COUNT = 300;
static char* translatedMessage[MAX_MESSAGE_COUNT];
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

void initTranslator(void)
{
	memset(translatedMessage, 0, MAX_MESSAGE_COUNT * sizeof(char*));
}

const char* translate(const char *__domainname, const char *__msgid)
{
	static int counter = 0;
	QString messageString = QApplication::translate("",  __msgid);

	counter++;

	if (counter >= MAX_MESSAGE_COUNT)
		counter = 0;

	if (translatedMessage[counter])
		delete[] translatedMessage[counter];

	translatedMessage[counter] = new char[messageString.toUtf8().length() + 1];
	strcpy(translatedMessage[counter], messageString.toUtf8().constData());

	return translatedMessage[counter];
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

void destroyTranslator(void)
{
	for (int counter = 0; counter < MAX_MESSAGE_COUNT; counter++)
	{
		if (translatedMessage[counter])
			delete[] translatedMessage[counter];
	}

	memset(translatedMessage, 0, MAX_MESSAGE_COUNT * sizeof(char*));
}

void getUIDescription(char* desc)
{
	sprintf(desc, "Qt4 (%s)", qVersion());
}
