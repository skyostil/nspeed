# basicmodel.py

import Numeric as N
import dom3ds


def _colorf(color,alpha,default):
        if color is None:
                return (default,default,default,alpha)
        elif type(color) is dom3ds.COLOR_24:
                return (color.red/255.0,color.green/255.0,
                        color.blue/255.0,alpha)
        else:
                return (color.red,color.green,color.blue,alpha)


def _pctf(pct,default):
        if pct is None:
                return default
        elif type(pct) is dom3ds.INT_PERCENTAGE:
                return pct.value/100.0
        else:
                return pct.value


def _enorm(vecarray):
        return N.sqrt(vecarray[:,0]**2 + vecarray[:,1]**2 + vecarray[:,2]**2)


class BasicMaterial(object):
        def __init__(self,name,ambient,diffuse,specular,
                     shininess,texture,twosided):
                self.name = name
                self.ambient = ambient
                self.diffuse = diffuse
                self.specular = specular
                self.shininess = shininess
                self.texture = texture
                self.twosided = twosided


class BasicMesh(object):
        def __init__(self,name,facearray,pointarray,
                     smarray,tvertarray,modmatrix,matarrays):
                self.name = name
                self.facearray = facearray
                self.pointarray = pointarray
                self.smarray = smarray
                self.tvertarray = tvertarray
                self.matarrays = matarrays
                self.transform_points(modmatrix)
                self.calculate_vertices()

        def transform_points(self,matrix):
                if max(abs(matrix-N.identity(4,N.Float)).flat) < 1e-10:
                        return
                n = len(self.pointarray)
                pt = N.ones((n,4),N.Float)
                pt[:,:3] = self.pointarray
                tpt = N.transpose(N.matrixmultiply(matrix,N.transpose(pt)))
                self.pointarray = N.array(tpt[:,:3]/tpt[:,3:4])

        def calculate_vertices(self):
                facearray = self.facearray
                pointarray = self.pointarray
                smarray = self.smarray
                tvertarray = self.tvertarray

                m = len(self.facearray)
                p = N.zeros((m,3),N.Float)
                rnorms = N.zeros((m*3,3),N.Float)
                fnorms = N.zeros((m*3,3),N.Float)
                points = N.take(pointarray,facearray.flat)

                A = N.take(pointarray,facearray[:,0])
                B = N.take(pointarray,facearray[:,1])
                C = N.take(pointarray,facearray[:,2])
                a = C - B
                b = A - C
                c = B - A
                p[:,0] = c[:,2]*b[:,1]-c[:,1]*b[:,2]
                p[:,1] = c[:,0]*b[:,2]-c[:,2]*b[:,0]
                p[:,2] = c[:,1]*b[:,0]-c[:,0]*b[:,1]
                la = _enorm(a)
                lb = _enorm(b)
                lc = _enorm(c)
                lp = _enorm(p)

                sinA = N.clip(lp/lb/lc,-1.0,1.0)
                sinB = N.clip(lp/la/lc,-1.0,1.0)
                sinC = N.clip(lp/la/lb,-1.0,1.0)
                sinA2 = sinA*sinA
                sinB2 = sinB*sinB
                sinC2 = sinC*sinC
                angA = N.arcsin(sinA)
                angB = N.arcsin(sinB)
                angC = N.arcsin(sinC)
                angA = N.where(sinA2 > sinB2 + sinC2, N.pi - angA, angA)
                angB = N.where(sinB2 > sinA2 + sinC2, N.pi - angB, angB)
                angC = N.where(sinC2 > sinA2 + sinB2, N.pi - angC, angC)

                rnorms[0::3] = p*(angA/lp)[:,N.NewAxis]
                rnorms[1::3] = p*(angB/lp)[:,N.NewAxis]
                rnorms[2::3] = p*(angC/lp)[:,N.NewAxis]

                vecmap = {}
                for i in xrange(3*m):
                        tp = tuple(points[i])
                        if vecmap.has_key(tp):
                                vecmap[tp].append(i)
                        else:
                                vecmap[tp] = [i]

                exarray = N.zeros(3*m,N.UnsignedInt32)
                if smarray:
                        exarray[0::3] = exarray[1::3] = exarray[2::3] = smarray

                def isg(x):
                        return exarray[i] & exarray[x]

                for i in xrange(3*m):
                        rgroup = vecmap[tuple(points[i])]
                        group = [ x for x in rgroup if exarray[i]&exarray[x] ]
                        if not group:
                                group = [i]
                        q = N.sum(N.take(rnorms,group))
                        lq = N.sqrt(N.dot(q,q))
                        if lq: q /= lq
                        fnorms[i] = q

                if tvertarray:
                        tverts = N.take(tvertarray,facearray.flat)
                else:
                        tverts = None

                self.points = points
                self.norms = fnorms
                self.tverts = tverts


class BasicModel(object):
        meshclass = BasicMesh
        matclass = BasicMaterial

        def __init__(self,dom,load_texture):
                self.load_texture = load_texture
                try:
                        self.extract_materials(dom)
                        self.extract_meshes(dom)
                finally:
                        del self.load_texture

        def extract_materials(self,dom):
                self.materials = {}
                for mat in dom.mdata.materials:
                        m = self.create_material(mat)
                        self.materials[m.name] = m

        def extract_meshes(self,dom):
                self.meshes = []
                for nobj in dom.mdata.objects:
                        obj = nobj.obj
                        if type(obj) is not dom3ds.N_TRI_OBJECT:
                                continue
                        self.meshes.append(self.create_mesh(nobj.name,obj))

        def create_material(self,mat):
                name = mat.name.value
                alpha = 1.0 - _pctf(mat.transparency
                                    and mat.transparency.pct,0.0)
                ambient = _colorf(mat.ambient
                                  and mat.ambient.color,alpha,0.2)
                diffuse = _colorf(mat.diffuse
                                  and mat.diffuse.color,alpha,0.8)
                specular = _colorf(mat.specular
                                   and mat.specular.color,alpha,0.0)
                shininess = _pctf(mat.shininess
                                  and mat.shininess.pct,0.0)
                texture = (mat.texmap
                           and self.load_texture(mat.texmap.filename.value))
                twosided = bool(mat.two_side)
                return self.matclass(name,ambient,diffuse,specular,
                                     shininess,texture,twosided)

        def create_mesh(self,name,nto):
                facearray = N.array(nto.faces.array[:,:3])
                smarray = nto.faces.smoothing and nto.faces.smoothing.array
                pointarray = nto.points.array
                tvertarray = nto.texverts and nto.texverts.array
                matrix = nto.matrix and nto.matrix.array
                matarrays = []
                for m in nto.faces.materials:
                        matarrays.append((self.materials[m.name],m.array))
                return self.meshclass(name,facearray,pointarray,smarray,
                                      tvertarray,matrix,matarrays)
