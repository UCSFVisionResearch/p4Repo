H = fspecial('disk',4);
If = imfilter(I,H);
[xb,yb] = OverlayPlot(If,0.61,16);
