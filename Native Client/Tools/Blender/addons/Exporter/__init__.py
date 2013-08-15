#This blender script exports selected objects in the scene to Starfall's format for 3D models.
bl_info = {
    "name":         "Starfall Exporter",
    "author":       "Mark Farrell",
    "blender":      (2,6,8),
    "version":      (0,0,1),
    "location":     "File > Import-Export",
    "description":  "A JSON model format for Starfall.",
    "category":     "Import-Export"
}
         
import bpy
import json
from bpy_extras.io_utils import ExportHelper
 
class ExportStarfallFormat(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_starfall.json";
    bl_label        = "Starfall Exporter";
    bl_options      = {'PRESET'};
     
    filename_ext    = ".json";
    
    #Formats the blender tessface as a dictionary, using vertex at indices a, b, and c
    #Note: Face.vertices are indices, Mesh.vertices are vertex values
    def formatFace(self, mesh, face, a, b, c): 
        return {
                "face" : { 
                    "vertices" : [(lambda x: [str(round(y,5)) for y in mesh.vertices[face.vertices[x]].co])(x) for x in [a,b,c]],
                    "material_index" : str(face.material_index),
                    "normal" : [str(round(x,5)) for x in face.normal]
                } 
        };
     
    def execute(self, context):
        bpy.ops.object.mode_set(mode="OBJECT");
        
        obs = context.selected_objects;
        objectList = [];
        
        if not obs: 
            raise NameError("No selected objects.");
            
        for ob in obs:   

            if ob.type == "MESH":
                materialList = [];
                faceList = [];
                    
                mesh = ob.data;
                mesh.update(calc_tessface=True); #Tesselate the meshes polygons; convert them into triangles 
                
                for material in mesh.materials: 
                    materialList.append({ 
                       "material" : {
                           "name" : str(material.name),
                           "alpha" : str(material.alpha),
                           "diffuse_color": [str(round(x, 5)) for x in material.diffuse_color],
                           "specular_color" : [str(round(x, 5)) for x in material.specular_color]
                        }
                    }); 

                for face in mesh.tessfaces:
                    if len(face.vertices) == 3:
                        faceList.append(self.formatFace(mesh, face, 0, 1, 2));
                    else: #4 vertices
                        faceList.append(self.formatFace(mesh, face, 0, 1, 2));
                        faceList.append(self.formatFace(mesh, face, 0, 2, 3));
                            
                objectList.append({ 
                   "mesh" : {
                       "name" : ob.name, 
                       "faces" : faceList,
                       "materials" : materialList
                   }
                });   
                
            else:
                print("No export object for %s" % ob.type);
             
        document = { "meshes" : objectList };
                    
        file=open(self.filepath, "w");
        file.write(json.dumps(document));
        file.close();   
        print(json.dumps(document, indent=2, separators=(","," : "), sort_keys=True));
        
        return {'FINISHED'};
 
def menu_func(self, context):
    self.layout.operator(ExportStarfallFormat.bl_idname, text="Starfall JSON format(.json)");
 
def register():
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func);
     
def unregister():
    bpy.utils.unregister_module(__name__);
    bpy.types.INFO_MT_file_export.remove(menu_func);
 
if __name__ == "__main__":
    register()