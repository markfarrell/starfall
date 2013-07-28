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