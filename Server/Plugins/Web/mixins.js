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

_.mixin({
    eachObject : function(obj, func) {

        if(typeof(obj) !== "object") {
            return;
        }
        
        for(var prop in obj) {
            if(!isNaN(parseInt(prop))) { //call the function for the nested object of an "index" key. e.g. in the case of { "0" : { "obj" : {} } } treat "obj" as the key to call the function for. 
                _.some(obj[prop], function(child,childProp) { 
                    func(child,childProp);
                    return true; //assume there can only be one nested object for an "index" key.
                });
            }
            else if(typeof(obj[prop]) === "object") {
                func(obj[prop],prop);
            }
        }
    },
    arrayToObject : function(args) {
        if(_.isArray(args)) {
            if(args.length > 1) {
                var ret = {};
                _.each(args, function(elem, index) {
                    ret[index] = _.arrayToObject(elem); //recursively turn array(s) into an object
                });
                return ret;
            } else {
                return args[0];   
            }
        } 
        return args;
    },
    deepClone : function(obj) {
        return JSON.parse(JSON.stringify(obj));     
    }
});