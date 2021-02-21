# Fonctionnement Moteur3D

**<h2>Cours 1 - Introduction & algorithme de dessin de ligne de Bresenham</h2>**

**Les buts du cours :**<br>
Écrire le programme qui peut :
- [x] lire le fichier .obj et stocker le contenu dans les varibles appropriées
- [x] afficher le nuage de points sur l'image du format .tga
- [x] afficher les points connectés les uns aux autres à l'aide d'algorithme de tracé de segment de Bresenham

|Affichage des points récupérés dans la console | Image avec des points uniquement | Résultat de l'algorithme de Bresenham |
|:---:|:---:|:---:|
|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/points.png" width="265">  |  <img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/head_points.png" width="350"> | <img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/head_lines.png" width="300"> |

**<h2>Cours 2 - Rastérisation triangulaire & élimination des faces cachées (back face culling)</h2>**
**Les buts du cours :**<br>
Écrire le programme qui peut :
- [x] rastériser le triangle en calculant la boîte englobante et les coordonnées barycentriques
- [x] éliminer les faces cachées en prenant en compte l'intensité de l'éclairage 

|Rastérisation triangulaire | Élimination des faces cachées |
|:---:|:---:|
|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/triangle_rasterization.png" width="265">  |  <img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/back_face_culling.png" width="265"> |

**<h2>Cours 3 - Suppression des faces cachées (zBuffer)</h2>**
**Les buts du cours :**<br>
Écrire le programme qui peut :
- [x] utiliser zBuffer algorithme
- [x] obtenir les couleurs des pixel de la texture pour les mettre sur l'image rendu.

|Avec le zBuffer | Ajout de la texture | 
|:---:|:---:|
| <img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/zbuffer.png" width="265"> |<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/with_textures.png" width="265">  

**<h2>Cours 4 - Projection en perspective</h2>**
**Les buts du cours :**<br>
Écrire le programme qui peut :
- [x] appliquer la transformations affines 3D sur l'image pour l'afficher en perspective

|Perspective projection |
|:---:|
<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/perspective_projection.png" width="300">

**<h2>Cours 5 - Bouger la caméra</h2>**
<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/camera.png" width="300">

**<h2>Cours 6 - Shaders pour le logiciel de rendu </h2>**
|Les intensités ne peuvent avoir que 6 valeurs | Specular mapping | 
|:---:|:---:|
|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/diffIntensities.png" width="300">|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/phong.png" width="300">|

**<h2>Cours 6bis - mappage normal de l'espace tangent </h2>**
<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/tangent.png" width="300">

**<h2>Cours 7 - Shadow mapping </h2>**
|Shadow shader | Résultat du rendu d'ombre | 
|:---:|:---:|
|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/shadow_bw.png" width="300">|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/shadow.png" width="300">|


**<h2>Cours 8 - Ambient occlusion </h2>**
|Rendu d'occlusion ambiante  | Rendu avec contraste | 
|:---:|:---:|
|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/ambient.png" width="300">|<img src="https://github.com/ShkAnna/Fonctionnement_Moteur3D/blob/main/images/ambient_contrast.png" width="300">|

