//Copyright (c) 2013 Mark Farrell


var System =  {
    Include : function(path) {
        if(!Include(path)) {
            throw "This script has failed to execute.";
        }
    },
    Data : function(path) {
        return Data(path);
    },
    Print : function(message) {
        Print(message);
    },
    Xml2Json : function(document) {
         var json = {};
         var recurse = function(obj, elem) {
            elem.eachChild(function(child, index, array) {
                if(!obj.hasOwnProperty(child.name)) {
                    obj[child.name] = child.attr;
                    recurse(obj[child.name], child);
                } else {
                    obj[index] = {};
                    obj[index][child.name] = child.attr;
                    recurse(obj[index][child.name], child);
                    if(index == array.length-1) {
                        if(obj.hasOwnProperty(child.name)) { //collect all elements with the same name, including the first element before the duplicate occurred
                            obj[0] = {};
                            obj[0][child.name] = obj[child.name];
                            delete obj[child.name];
                        }
                    }
                }
            });

        };
        recurse(json, document);
        return json;
    },
    Log : function(name, body, foot) {
        if(typeof foot === "undefined") {
            foot = {};
        }
        Http(JSON.stringify({ head: { datetime: Time(), name: name}, body: body, foot: foot },null));
    }
};

try {
    System.Include("Common/lib/underscore/underscore-min.js");
    System.Include("Common/lib/xml/sax.js");
    System.Include("Common/lib/xml/xmldoc.js");
    System.Include("Common/mixins.js");
    Main();
} catch(err) {
    System.Print(err);
}