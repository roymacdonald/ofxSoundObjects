//
//  ofxSoundRendererUtils.h
//  example-matrixMixer_tbb
//
//  Created by Roy Macdonald on 8/18/19.
//
//

#pragma once

namespace ofxSoundRendererUtils{
	
	static void addLineToMesh(ofVboMesh& mesh, const glm::vec3& p0, const glm::vec3& p1, const ofFloatColor& color){
		auto i = mesh.getVertices().size();
		mesh.addVertex(p0);
		mesh.addVertex(p1);
		mesh.addIndex(i);
		mesh.addIndex(i+1);
		mesh.addColors({color,color});
	}
	static void addRectToMesh(ofVboMesh& mesh, const ofRectangle& r, const ofFloatColor& color, bool bAddLinesIndices){
//			std::cout << "addRectToMesh " << r << std::endl;
		auto i = mesh.getVertices().size();
		
		mesh.addVertex(r.getTopLeft());
		mesh.addVertex(r.getTopRight());
		mesh.addVertex(r.getBottomRight());
		mesh.addVertex(r.getBottomLeft());
		mesh.addColors({color,color,color,color});
		
		if(bAddLinesIndices){
			mesh.addIndex(i);
			mesh.addIndex(i+1);
			
			mesh.addIndex(i+1);
			mesh.addIndex(i+2);
			
			mesh.addIndex(i+2);
			mesh.addIndex(i+3);
			
			mesh.addIndex(i+3);
			mesh.addIndex(i);
		}else{
			mesh.addIndex(i);
			mesh.addIndex(i+1);
			mesh.addIndex(i+2);
			
			mesh.addIndex(i);
			mesh.addIndex(i+2);
			mesh.addIndex(i+3);
			
		}
	}
}
