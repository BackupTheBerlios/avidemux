TGT=../avidemux_icon_small.inc \
../gnome-calculator.inc \
../gnome-calculator_small.inc \
../stock_allow-effects.inc \
../stock_filters-aging.inc

../%.inc : %.png	
	echo $<
	gdk-pixbuf-csource --raw --name=`basename $@ .inc` $< > $@
all : $(TGT)

clean:
	rm -f $(TGT) 
