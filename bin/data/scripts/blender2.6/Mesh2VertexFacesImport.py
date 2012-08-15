
import bpy; 
from bpy import *; 
import bmesh;

# prepare a scene
scn = bpy.context.scene
scn.frame_start = 0
scn.frame_end = 442
primera = 1


# create obj and add to scene
     
me = bpy.data.meshes.new('animacion_mesh') 
ob = bpy.data.objects.new('animacion', me) 
ob.location = bpy.context.scene.cursor_location
bpy.context.scene.objects.link(ob)   


ob.select = True

# loop through frames

for activeframe in range(50, 53):

    file = open('/Users/welovecode/Desktop/DATA/frame'+str(activeframe)+'.txt','r')
    
    mydata = {} 
    vertexdata = []
    facedata = []
    counter=0
    c= 0
    
    # Vertex
    
    for line in file.readlines():
        
        mydata[c] = line.split(',');
        
        rX = float(mydata[c][1])-50; 
        rY = float(mydata[c][2])-100;
        rZ = float(mydata[c][3])-650;
        
        nextVertex = rX/10,rZ/10,-rY/10
        vertexdata.append(nextVertex);
        
        c=c+1
        

    
    for i in range(0, c-55):
        
        #mydata[counter] = line.split(',');
        
        ap = counter
        bp = counter+1
        cp = counter+54
        dp = counter+55
        
        if (float(mydata[ap][1]) > 0 and float(mydata[bp][1]) > 0 and float(mydata[cp][1]) > 0) or (float(mydata[ap][2]) > 0 and float(mydata[bp][2]) > 0 and float(mydata[cp][3]) > 0) or (float(mydata[ap][3]) and float(mydata[bp][3]) > 0 and float(mydata[cp][3])):
            nextFace = bp,cp,ap
            facedata.append(nextFace);
        if (float(mydata[dp][1]) > 0 and float(mydata[bp][1]) > 0 and float(mydata[cp][1]) > 0) or (float(mydata[dp][2]) > 0 and float(mydata[bp][2]) > 0 and float(mydata[cp][3]) > 0) or (float(mydata[dp][3]) and float(mydata[bp][3]) > 0 and float(mydata[cp][3])):
            nextFace =dp,cp,bp
            facedata.append(nextFace);
        
        counter=counter+1
    
    bpy.context.scene.frame_set(activeframe)
    
    if primera == 1:
        me.from_pydata(vertexdata,[], facedata)
        me.update(calc_edges=True)   
        
        primera = 0
    else:
        
        #ob.keyframe_insert(data_path="hide_render", frame=activeframe) 
        #bpy.ops.anim.keyframe_insert(type='shape', confirm_success=True)
        c = 0;
        f= 0;
        for vert in ob.data.vertices:
            vert.keyframe_insert("co", frame=activeframe-1)
            
            nX = float(mydata[c][1])-50; 
            nY = float(mydata[c][2])-100;
            nZ = float(mydata[c][3])-650;      
            
            pVertex = nX/10,nZ/10,-nY/10
            if float(mydata[c][1]) > 0 and float(mydata[c][2]) > 0 and float(mydata[c][3]) > 0:
                vert.co = pVertex
            else:
                vert.co = vert.co
            
            vert.keyframe_insert("co", frame=activeframe)
            c = c+1

            
    # anim
    
    #ob.hide = True
    #ob.hide_render = True
    #ob.keyframe_insert(data_path="hide", frame=0) 
    #ob.keyframe_insert(data_path="hide_render", frame=0) 
    #ob.hide = False 
    #ob.hide_render = False
    #ob.keyframe_insert(data_path="hide", frame=activeframe)
    #ob.keyframe_insert(data_path="hide_render", frame=activeframe) 
    #ob.hide = True
    #ob.hide_render = True
    #ob.keyframe_insert(data_path="hide", frame=activeframe+1) 
    #ob.keyframe_insert(data_path="hide_render", frame=activeframe+1) 
    
     
    file.close()




# select the created object
# bpy.ops.object.select_name(name="myTriangle")

# do something with the object. A rotation, in this case
# bpy.ops.transform.rotate(value=(-0.5*pi, ), axis=(-1, 0, 0))

# create keyframe
# bpy.ops.anim.keyframe_insert_menu(type='Rotation')