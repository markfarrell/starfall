//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

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
                         "Common/mixins.js",
                         "Web/app/app.js",
                         "Web/app/components.appearance.js"])
            .map(function(elem) { return ["/"+elem, elem]; })
            .union([["/","Web/index.html"]])
            .object()
            .value();
}

