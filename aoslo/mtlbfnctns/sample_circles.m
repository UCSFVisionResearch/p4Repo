width = 300;
height = 150;
cx = 150;
cy = 75;
rx = 149;
ry = 74;

A = rand(height,width);

mask = bsxfun(@plus, (((1:width) - cx).^2)./(rx^2), ((transpose(1:height) - cy).^2)./ry^2) < 1;

Samp = A .* mask;