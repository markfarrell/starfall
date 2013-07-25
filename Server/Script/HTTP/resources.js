function Main() {
	return _.chain(["img/logo_small.png",
			             "jquery.mobile.custom/images/ajax-loader.gif",
			             "jquery.mobile.custom/images/icons-18-black.png",
                         "jquery.mobile.custom/images/icons-18-white.png",
                         "jquery.mobile.custom/images/icons-36-black.png",
                         "jquery.mobile.custom/images/icons-36-white.png",
                         "jquery.mobile.custom/jquery.mobile.custom.theme.css",
                         "jquery.mobile.custom/jquery.mobile.custom.structure.css",
                         "jquery.mobile.custom/jquery.mobile.custom.min.js",
                         "jquery/jquery-1.9.1.js",
                         "underscore/underscore-min.js",
			                   "items.js",
                         "mixins.js",
                         "app/app.js",
                         "app/components.appearance.js"])
            .map(function(elem) { return ["/"+elem, elem]; })
            .union([["/","index.html"]])
            .object()
            .value();
}

