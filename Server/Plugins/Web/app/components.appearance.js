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


var Components = Components || {};
Components.Appearance = Components.Appearance || {};


Components.Appearance.construct = function() {

    function Part() {
        this.name = "";
        this.data = { style: {}, part: {}, mesh: {}, slot: {} }; //store the original object data pertaining to appearance for each part, so that a final object can be assembled later for item data 
        this.states = { on: false };
        this.ids = {};
        this.colors = [];
        
    }
    
    Part.prototype = {
        render : function() {
            var hexColors = [];
            var html = "";
            _.each(this.colors,function(elem) {
                hexColors.push({ids: elem.ids, red: elem.red, green: elem.green, blue: elem.blue, hex: elem.toHex()});
            });
            //Render color object
            html = _.template(($("#template-object-part").html()), { partName: this.name, partIDs: this.ids, hexColors: hexColors });
            return html;
        },
        register : function() {
            $("#"+this.ids.toggle).change((function() {
                if($("#"+this.ids.toggle).val() == "on") {    
                    this.states.on = true;
                }else{       
                    this.states.on = false;
                }
            }).bind(this));    
        }
    };
    
    
    function Color() {
        this.ids = {};
        this.data = { material: {} };
        this.number = 0; //address in style sheet
        this.red = 0;
        this.green = 0;
        this.blue = 0;  
    }
    
    Color.prototype = { 
        toHex : function() {
            var color = { red : parseInt(this.red), green : parseInt(this.green), blue : parseInt(this.blue) };
            for(var c in color) {
                if(isNaN(color[c])) { //parse error
                    color[c] = 0;
                }
                color[c] = color[c].toString(16); //255->FF
                if(color[c].length == 1) {
                    color[c] = "0" + color[c]; //3 -> 03
                }
            }
            return "#"+color.red+color.green+color.blue; //e.g. #+FF+FF+FF = #FFFFFF
        },
        current : function() {
            var selectedColor = new Color();
            selectedColor.ids = this.ids;
            selectedColor.data = this.data;
            selectedColor.number = this.number;
            selectedColor.red = $("#"+this.ids.sliders.red).slider().val();
            selectedColor.green = $("#"+this.ids.sliders.green).slider().val();
            selectedColor.blue = $("#"+this.ids.sliders.blue).slider().val();
            return selectedColor;
        },
        register : function() {
            $("#"+this.ids.buttons.apply).click((function() {
                $("#"+this.ids.anchor).css("background-color", this.current().toHex());
            }).bind(this));
            $("#"+this.ids.buttons.revert).click((function() {
                $("#"+this.ids.anchor).css("background-color", this.toHex());
            }).bind(this));
        }  
    };
    
    function Model(baseID, obj) { 
        this.appearance = obj;
        this.parts = [];
        var self = this;
        _.eachObject(this.appearance.styles, function(style) {
            if(style.name == "default") {
                var i = 0;
                _.eachObject(obj.slots, function(slot) {
                    _.eachObject(slot, function(mesh) {
                        _.eachObject(mesh, function(part) {
                            var partComponent = new Part();
                            partComponent.name = part.name;
                            partComponent.data.style = style;
                            partComponent.data.part = part;
                            partComponent.data.mesh = mesh;
                            partComponent.data.slot = slot;
                            partComponent.ids.li = baseID + "_" + "part_"+i;
                            partComponent.ids.toggle =  partComponent.ids.li + "_toggle";
                            var j = 0;
                            _.eachObject(part, function(material) {
                                _.eachObject(style, function(styleColor) {
                                    if(styleColor.number == material.color) {
                                        var color = new Color();
                                        color.data.material = material;
                                        color.number = styleColor.number;
                                        color.red = styleColor.red;
                                        color.green = styleColor.green;
                                        color.blue = styleColor.blue;
                                        color.ids.anchor = partComponent.ids.li + "_color_" + j;
                                        color.ids.popup = color.ids.anchor + "_popup";
                                        color.ids.sliders = {};
                                        color.ids.sliders.red = color.ids.popup + "_input_red";
                                        color.ids.sliders.green = color.ids.popup + "_input_green";
                                        color.ids.sliders.blue = color.ids.popup + "_input_blue";
                                        color.ids.buttons = {};
                                        color.ids.buttons.apply = color.ids.popup + "_button_apply";
                                        color.ids.buttons.revert = color.ids.popup + "_button_revert";
                                        partComponent.colors.push(color);
                                        j++;
                                    }
                                });
                            });
                            i++;
                            self.parts.push(partComponent); //TODO: iterate over model's parts to add controls 
                        });
                    });
                });
            }
        });
    }
    
    Model.prototype = {
        render : function() {
            return _.reduce(this.parts, function(memo, part) { return memo + part.render(); }, "");
        },
        data : function() {
            var self = this;
            var ret = {};
            var style = [];
            var nextNumber = 0;
            var slotArray = [];
            _.eachObject(_.deepClone(this.appearance.slots), function(slot) {
                var meshArray = [];
                _.eachObject(slot, function(mesh) { 
                    var partArray = [];
                    _.eachObject(mesh, function(part) {
                       _.every(self.parts, function(partComponent) { 
                            if(partComponent.states.on) {
                                if(JSON.stringify(partComponent.data.part) === JSON.stringify(part)) {
                                    var includeColors = _.chain(partComponent.colors)
                                        .filter(function(currentColorComponent) {
                                            return _.every(style, function(styleElem) { 
                                                if(_.isEqual(_.pick(currentColorComponent.current(), "red", "green", "blue"), _.pick(styleElem.color,"red","green","blue"))) {
                                                    _.eachObject(part, function(material) {
                                                        if(_.isEqual(currentColorComponent.data.material.name, material.name)) { 
                                                                material.color = styleElem.color.number;
                                                        }
                                                    });
                                                    return false;
                                                }
                                                return true;
                                            });
                                        })
                                        .map(function(color) { 
                                            var nextColor = _.pick(color.current(), "red", "green", "blue");
                                            nextColor.number = "-1";
                                            _.eachObject(part, function(material) {
                                               if(_.isEqual(color.data.material.name, material.name)) {
                                                    material.color = ""+nextNumber;
                                                    nextColor.number = ""+nextNumber;
                                                    nextNumber++;
                                               }
                                            });

                                            return { "color" : nextColor };
                                        })
                                        .value();
                                    style = style.concat(includeColors);
                                    partArray.push({"part" : part});
                                    return false; //break
                                }
                            }
                            return true; 
                        });
                    });
                    if(partArray.length > 0) {
                        meshArray.push({"mesh" : _.extend(_.pick(mesh,"name"), _.arrayToObject(partArray))});
                    }
                });
                if(meshArray.length > 0) {
                    slotArray.push({"slot" : _.extend(_.pick(slot,"name"), _.arrayToObject(meshArray)) });
                }
            });
            ret.slots = _.arrayToObject(slotArray);
            ret.styles = { "style" : _.extend( { "name" : "default" }, _.arrayToObject(style)) };
            return ret;
        }
    };
    
    function Controls(model) {
        this.model = model;
    }
    
    Controls.prototype = {
        colors : function() {
            return _.chain(this.model.parts)
                    .reduce(function(memo, part) { return memo.concat(part.colors); }, [])
                    .map(function(color) { return color.register.bind(color); })
                    .value();
        },
        parts : function() {
            return _.map(this.model.parts, function(part) { return part.register.bind(part); }); 
        },
        all : function() {
            return _.union(this.colors(), this.parts());
        }
    };
    
    function View(model) {
        this.model = model;
    }
    
    View.prototype = { 
        all : function() {
            return _.reduce(this.model.parts, function(memo, part) { return memo + part.render() }, "");
        }
    };

    function create(baseID, obj) {
        var model = new Model(baseID, obj);
        var view = new View(model);
        var controls = new Controls(model);
        return { 
            model : model,
            view : view,
            controls : controls
        };
    }
    
    this.create = create;
    this.Controls = Controls;
    this.Model = Model;   
};
Components.Appearance.construct();


