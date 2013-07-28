
var Items = Items || {};
(function() {
    function Apparel(obj) {
        _.extend(this,obj);
    }
    this.Apparel = Apparel;
}).call(Items);


var Inventory = Inventory || function() {
    this.items = [];
    this.types = [{ type: "apparel", class: Items.Apparel }];
    
    _.each(this.default(), (function(item) {
        if(_.has(item,"type")) {
           var classFunction = _.chain(this.types).findWhere({type: item.type}).value().class;
           if(!_.isUndefined(classFunction)) {
                this.items.push(new classFunction(item));
            }
        }
    }).bind(this));
};



Inventory.prototype.default = function() {
        var defaultInventory = JSON.parse(System.Data("Entity/inventory.json"));
        if(_.isArray(defaultInventory)) {
            return defaultInventory;
        }
        return [];   
};
