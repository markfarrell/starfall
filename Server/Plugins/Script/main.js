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
    Main();
} catch(err) {
    System.Print(err);
}