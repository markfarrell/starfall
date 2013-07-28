/* Requires: Underscore.js */
/**
 * Description: Idea taken from a stackexchange namespace discussion.
 * Link: http://stackoverflow.com/questions/881515/javascript-namespace-declaration
 */

var Items = Items || {};

Items.construct = function () {

    

    // pass -> (["a","b", "c", ...], { idString: ..., subCounter: ...})
    function Instance() { }

    Instance.Form = function (ids,appearance) {
        this.ids = ids;
        this.model = new Instance.Form.Model(this);
        this.view = new Instance.Form.View(this);
        this.controls = new Instance.Form.Controls(this);
        this.appearance = appearance;
    };

    Instance.Form.prototype = {
        ids : {},
        itemTypes : [],

        getItemTypes : function() {
            return this.itemTypes;
        }
    };

    Instance.Form.Model = function(p) { this.parent = p; }
    Instance.Form.Model.prototype = {

        render : function() {
            var html = "";
            var selection = this.parent.controls.getSelection();
            var selectedItemTypeInputs = [];
            for(var key in selection) {
                var inputID = this.parent.ids.selectedItemTypeList+"_"+key;
                selectedItemTypeInputs.push({ inputID : inputID, key: key });
                var defaultValue = selection[key].value;
                if(defaultValue == undefined) {
                    defaultValue = "";
                }
                html += "<li>";
                html += "<table width='100%'>";
                html += "<tr>";
                html += "<td width='50%'>";
                if(key.length > 0) {
                    html += key.charAt(0).toUpperCase() + key.substr(1);
                }
                html += "</td>";
                html += "<td width='50%'>";
                html += "<input id='"+inputID+"' type='text' value='"+defaultValue+"' />";
                html += "</td>";
                html += "</tr>";
                html += "</table>";
                html +="</li>";
            }
            this.parent.ids.selectedItemTypeInputs = selectedItemTypeInputs;
            return html;
        },

    };

    Instance.Form.View = function(p) { this.parent = p; }
    Instance.Form.View.prototype = {
        update : function() {
            var selection = this.parent.controls.getSelection();
            var contentFunc = (function(selectedItemType) {
                var content = _.template(($("#template-object-item-select").html()), { form : this.parent });
                if($("#"+this.parent.ids.menu).length > 0) { //does exist?
                    $("#"+this.parent.ids.menu).remove();
                }
                $("#"+this.parent.ids.root).append(content);
                $("#"+this.parent.ids.root).trigger("create");
                $("#"+this.parent.ids.root).trigger("updateLayout");
            }).bind(this);
            contentFunc(selection);
            selection = this.parent.controls.getSelection();
            if(_.size(selection) > 0) {
                contentFunc(selection); //load again if selection was found after loading the first time
            }
        },
    };


    Instance.Form.Controls = function(p) { this.parent = p; }
    Instance.Form.Controls.prototype = {
        onTransition : function() { },
        reset : function() {
            this.parent.view.update();
            this.registerOkButton();
            this.registerSelectChange();
            this.registerCancelButton();
            this.registerNewButton();
        },
        getSelection : function() {
            var selectedItemType = {}; //selected item type to create a new item instance
            var value = $("#"+this.parent.ids.select).val(); //could use option:selected
            if(value != undefined && value != null) {
                this.parent.itemTypes.forEach(function(elem) {
                    if(elem.name == value) {
                        selectedItemType = elem;
                    }
                });
            }
            return selectedItemType;
        },
        loadItemTypes : function(obj) {
            if(obj.itemTypes != undefined) {
                this.parent.itemTypes = [];
                for(var idx in obj.itemTypes) {
                    this.parent.itemTypes.push(obj.itemTypes[idx]);
                }
             }
        },
        ajaxSelectItemTypes : function() {
            $.ajax({
                    type: "POST",
                    url: "./ajax/itemtype/select",
                    cache: false,
                    dataType: "json"
                   }).done((function(obj) {
                        this.loadItemTypes(obj);
                        this.reset();
                  }).bind(this));
        },
        ajaxInsertItem : function(item) {
            $.ajax({
                    type: "POST",
                    url: "./ajax/item/new",
                    data: JSON.stringify(item),
                    cache: false,
                    dataType: "json"
            }).done((function(obj){/* Success */}).bind(this));
        },
        registerSelectChange : function () {
            var selectID = this.parent.ids.select;
            var listID = this.parent.ids.selectedItemTypeList;
            $("#"+selectID).change((function() {
                $("#"+listID+" li").remove(); //remove current items
                $("#"+listID).append(this.parent.model.render());
                $("#"+listID).listview("refresh");
                $("#"+listID).trigger("create");
                $("#"+listID).trigger("updateLayout");
            }).bind(this));
        },
        onCloseClick : function() {
            $("#"+this.parent.ids.menu).remove();
            $("#"+this.parent.ids.anchor).unbind("click"); //disable click
            $("#"+this.parent.ids.anchor).click(this.onOpenClick.bind(this)); //disable click
        },
        onOpenClick : function() {
            $("#"+this.parent.ids.anchor).unbind("click"); //disable click
            $("#"+this.parent.ids.anchor).click((function() { //click the anchor again to remove the item form
                    this.onCloseClick();
            }).bind(this));
            this.ajaxSelectItemTypes();
            this.reset();
        },
        registerAnchor : function() {
            $("#"+this.parent.ids.anchor).click(this.onOpenClick.bind(this));
        },
        registerCancelButton : function() {
            $("#"+this.parent.ids.cancelButton).unbind("click");
            $("#"+this.parent.ids.cancelButton).click((function() {
                this.onCloseClick();
            }).bind(this));
        },
        registerNewButton : function() {
            $("#"+this.parent.ids.newButton).click((function() {
                $("#"+this.parent.ids.menu).remove();
                this.onTransition();
            }).bind(this));
        },
        registerOkButton : function() {
            $("#"+this.parent.ids.okButton).click((function() {
                var selection = this.getSelection();
                var item = {};
                item.type = selection.name;
                item.data = this.parent.appearance.model.data();
                for(var idx in this.parent.ids.selectedItemTypeInputs) { //an array of objects containing IDs and keys
                    var obj = this.parent.ids.selectedItemTypeInputs[idx];
                    item[obj.key] = $("#"+obj.inputID).val();
                }
                console.log(item);
                this.ajaxInsertItem(item);
                this.onCloseClick();
            }).bind(this));
        }

    };

    function Types() {}

    Types.Form = function(ids) {
        this.ids = ids;
        this.model = new Types.Form.Model(this);
        this.view = new Types.Form.View(this);
        this.controls = new Types.Form.Controls(this);
    }

    Types.Form.prototype = {
        ids : {},
        dataTypes : ["String", "UInt32"]
    };

    Types.Form.Model = function(p) { this.parent = p; }
    Types.Form.Model.prototype = {
        render : function() {
            return _.template(($("#template-object-item-new").html()), { ids: this.parent.ids, dataTypes: this.parent.dataTypes });
        }
    };

    Types.Form.View = function(p) { this.parent = p; }
    Types.Form.View.prototype = {
        update : function() {
            var content = this.parent.model.render();
            $("#"+this.parent.ids.root).append(content);
            $("#"+this.parent.ids.root).trigger("create");
            $("#"+this.parent.ids.root).trigger("updateLayout");
            $("#"+this.parent.ids.propertyList).hide(); //hide when empty
        }
    };

    Types.Form.Controls = function(p) { this.parent = p; this.aspects = new Types.Form.Controls.Aspects(this); }
    Types.Form.Controls.prototype = {
        currentItemPrototype : {}, //new item type; to be added to the database

        reset : function() { //TODO: Break up into model and view
            this.currentItemPrototype = {};
            this.parent.view.update();
            this.registerAddPropertyButton();
            this.registerBackButton();
            this.registerCreateItemTypeButton();
        },
        registerAddPropertyButton : function() {
            $("#"+this.parent.ids.addPropertyButton).click((function() {
            var property = { name: $("#"+this.parent.ids.propertyInput).val().toLowerCase(), dataType: $("#"+this.parent.ids.dataTypes).val() };
                if(property.name.length > 0) {
                    if($("#"+this.parent.ids.propertyList).is(":hidden")) {
                        $("#"+this.parent.ids.propertyList).show();
                    }
                    this.currentItemPrototype[property.name] = {dataType: property.dataType, value : ""};
                    $("#"+this.parent.ids.propertyList).append("<li><table width='100%'><tr><td width='50%' align='left'>"+property.name+"</td><td width='50%' align='right'>"+property.dataType+"</td></tr></li>");
                    $("#"+this.parent.ids.propertyList).listview("refresh");
                    $("#"+this.parent.ids.propertyList).trigger("create");
                    $("#"+this.parent.ids.propertyList).trigger("updateLayout");
                }
            }).bind(this));
        },
        registerBackButton : function() {
            $("#"+this.parent.ids.cancelButton).click((function() { this.aspects.onClose(); }).bind(this));
        },
        ajaxNewItemType : function() {
            $.ajax({
                    type: "POST",
                    url: "./ajax/itemtype/new",
                    cache: false,
                    data: JSON.stringify(this.currentItemPrototype),
                    dataType: "json"
                    }).done((function(obj) {
                       this.aspects.onComplete(obj);
                    }).bind(this)); //TODO: Load item types when done.
        },
        registerCreateItemTypeButton : function() {
            $("#"+this.parent.ids.okButton).click((function() {
                var value = $("#"+this.parent.ids.nameInput).val();
                if(value.length > 0) {
                    this.currentItemPrototype.name = value.toLowerCase();
                    this.ajaxNewItemType();
                    this.aspects.onClose();
                }
            }).bind(this));
        }
    };

    Types.Form.Controls.Aspects = function(p) { this.parent = p; }
    Types.Form.Controls.Aspects.prototype = {
        onComplete : function(obj) {}, //when the Ajax request to add the item type to the database has finished
        onClose : function() {} //when the user clicks the ok button
    };

    this.Instance = Instance; //declare publicly
    this.Types = Types;
};
Items.construct();