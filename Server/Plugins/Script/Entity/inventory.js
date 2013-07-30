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

var Items = Items || {};
(function() {
    function Apparel(obj) {
        _.extend(this,obj);
        this.states = { equipped : false };
    }
    this.Apparel = Apparel;
}).call(Items);


var Inventory = Inventory || function() {
    this.items = [];
    this.types = [{ type: "apparel", class: Items.Apparel }]; 
    this.load();
    this.findEquipment();
    System.Print(JSON.stringify(this.equipped()));
};


Inventory.prototype.equipped = function() {
    var equippedStates = _.reduce(Appearance.slots.defaults(), function(memo, name) { memo[name] = false; return memo; }, {});
    _.each(this.items, function(item) {
        _.each(Appearance.slots.names(item.data), function(slotName) {
            if(equippedStates[slotName] === false) {
                if(item.states.equipped) {
                    equippedStates[slotName] = true;   
                }
            }
        });
    });
    return equippedStates;
};

Inventory.prototype.equip = function(item) {
    if(_.has(item, "states")) {
        if(_.has(item.states, "equipped")) {
            _.each(Appearance.slots.names(item.data), (function(slotName) {
                if(!this.equipped()[slotName]) { //use equipped() each time to check for updates
                    item.states.equipped = true;
                }
            }).bind(this));
        }
    }
};

/** 
 * Description: Try to equip every item that is apparel. Result: The first item found for each slot will be equipped. Conflicting items will not equip.
 * Usage: Used after the default inventory is loaded.
 */
Inventory.prototype.findEquipment = function() {
    _.each(this.items, (function(item) {
        if(item.type === "apparel") {
            this.equip(item);
        }
    }).bind(this));
};


Inventory.prototype.load = function() {
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


