from pathlib import Path
import json
import numpy as np
from PIL import Image

root=Path(__file__).resolve().parent/'build'
data=json.loads((root/'aim-ui-mesh.json').read_text())
width,height=data['width'],data['height']
atlas=np.frombuffer((root/'aim-ui-atlas.rgba').read_bytes(),dtype=np.uint8).reshape(data['atlasHeight'],data['atlasWidth'],4).astype(np.float32)/255
canvas=np.zeros((height,width,3),np.float32)
for draw in data['lists']:
    vertices=np.asarray(draw['vertices'],dtype=np.float64)
    colors=vertices[:,4].astype(np.uint32)
    colors=np.stack([colors&255,(colors>>8)&255,(colors>>16)&255,colors>>24],axis=1).astype(np.float32)/255
    indices=np.asarray(draw['indices'],dtype=int)
    for count,offset,base,left,top,right,bottom in draw['commands']:
        for tri in indices[int(offset):int(offset+count)].reshape(-1,3)+int(base):
            v=vertices[tri];c=colors[tri]
            x0=max(0,int(np.floor(v[:,0].min())),int(np.ceil(left)));y0=max(0,int(np.floor(v[:,1].min())),int(np.ceil(top)))
            x1=min(width,int(np.ceil(v[:,0].max())),int(np.ceil(right)));y1=min(height,int(np.ceil(v[:,1].max())),int(np.ceil(bottom)))
            if x1<=x0 or y1<=y0:continue
            denominator=(v[1,1]-v[2,1])*(v[0,0]-v[2,0])+(v[2,0]-v[1,0])*(v[0,1]-v[2,1])
            if abs(denominator)<1e-9:continue
            yy,xx=np.mgrid[y0:y1,x0:x1];xx=xx+.5;yy=yy+.5
            a=((v[1,1]-v[2,1])*(xx-v[2,0])+(v[2,0]-v[1,0])*(yy-v[2,1]))/denominator
            b=((v[2,1]-v[0,1])*(xx-v[2,0])+(v[0,0]-v[2,0])*(yy-v[2,1]))/denominator
            d=1-a-b;mask=(a>=0)&(b>=0)&(d>=0)
            if not mask.any():continue
            weights=np.stack([a[mask],b[mask],d[mask]],axis=1)
            uv=weights@v[:,2:4]
            tx=np.clip((uv[:,0]*data['atlasWidth']).astype(int),0,data['atlasWidth']-1)
            ty=np.clip((uv[:,1]*data['atlasHeight']).astype(int),0,data['atlasHeight']-1)
            rgba=(weights@c)*atlas[ty,tx]
            region=canvas[y0:y1,x0:x1]
            alpha=rgba[:,3:4]
            region[mask]=rgba[:,:3]*alpha+region[mask]*(1-alpha)
Image.fromarray(np.clip(canvas*255,0,255).astype(np.uint8)).save(root/'aim-ui-preview.png')
print('Saved standalone Arabic MP3 tab preview.')
