//Copyright (c) 2013 Mark Farrell

var Appearance = Appearance || {};

Appearance.construct = function() {
    this.defaults = function () { return System.Xml2Json(new XmlDocument(System.Data("Entity/appearance.xml"))); };
    this.slots = {};
    this.slots.names = function(obj) { 
        var slotNames = [];
        if(_.has(obj, "slots")) {
            _.eachObject(obj.slots, function(slot){
                if(_.has(slot, "name")) {
                    slotNames.push(slot.name);
                }
            });      
        }
        return slotNames;
    };
    this.slots.defaults = (function () {
        var ret = [];
        var obj = this.defaults();
        if(_.has(obj, "appearance")) {
            ret = ret.concat(this.slots.names(obj.appearance));
        }
        return ret;
    }).bind(Appearance);
 
};
Appearance.construct();


function Main() {
    System.Log("Appearance", Appearance.defaults());
}