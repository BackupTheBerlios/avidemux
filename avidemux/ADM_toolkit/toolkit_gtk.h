

int 		getRangeInMenu(GtkWidget * Menu);
void 		changeTextInMenu(GtkWidget *menu,int range,const char *text);
void 		UI_purge(void );
uint8_t 	getSelectionNumber(uint32_t nb,GtkWidget *tree  , GtkListStore 	*store,uint32_t *number);
uint8_t 	setSelectionNumber(uint32_t nb,GtkWidget *tree  , GtkListStore 	*store,uint32_t number);
int 		gtk_read_entry(GtkWidget *entry);
void 		gtk_write_entry(GtkWidget *entry, int value);
void 		gtk_write_entry_string(GtkWidget *entry, char *value);
float 		gtk_read_entry_float(GtkWidget *entry);
void		gtk_transient(GtkWidget *widget);
void 		gtk_write_entry_float(GtkWidget *entry, float value);
#define WID(x) lookup_widget(dialog,#x)
#define RADIO_SET(widget_name,value) gtk_toggle_button_set_active \
				( GTK_TOGGLE_BUTTON (WID(widget_name)), value)
				
#define RADIO_GET(widget_name) gtk_toggle_button_get_active \
				( GTK_TOGGLE_BUTTON (WID(widget_name)))

