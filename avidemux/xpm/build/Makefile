TGT=../avidemux_icon_small.inc \
../gnome-calculator.inc \
../gnome-calculator_small.inc \
../stock_allow-effects.inc \
../1.inc \
../2.inc \
../3.inc \
../4.inc \
../5.inc \
../6.inc \
../7.inc \
../preview.inc \
../output.inc \
../stock_filters-aging.inc

../%.inc : %.png	
	echo $<
	gdk-pixbuf-csource --raw --name=`basename $@ .inc` $< > $@
all : $(TGT)

clean:
	rm -f $(TGT) 
