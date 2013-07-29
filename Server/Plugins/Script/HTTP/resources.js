//The MIT License (MIT)
//
//Copyright (c) 2013 Mark Farrell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.



function Main() {
	return _.chain(["Web/img/logo_small.png",
			             "Web/jquery.mobile.custom/images/ajax-loader.gif",
			             "Web/jquery.mobile.custom/images/icons-18-black.png",
                         "Web/jquery.mobile.custom/images/icons-18-white.png",
                         "Web/jquery.mobile.custom/images/icons-36-black.png",
                         "Web/jquery.mobile.custom/images/icons-36-white.png",
                         "Web/jquery.mobile.custom/jquery.mobile.custom.theme.css",
                         "Web/jquery.mobile.custom/jquery.mobile.custom.structure.css",
                         "Web/jquery.mobile.custom/jquery.mobile.custom.min.js",
                         "Web/jquery/jquery-1.9.1.js",
                         "Common/lib/underscore/underscore-min.js",
			             "Web/items.js",
                         "Web/mixins.js",
                         "Web/app/app.js",
                         "Web/app/components.appearance.js"])
            .map(function(elem) { return ["/"+elem, elem]; })
            .union([["/","Web/index.html"]])
            .object()
            .value();
}

