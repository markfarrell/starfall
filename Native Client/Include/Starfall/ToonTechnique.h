//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall/Technique.h"
#include "Starfall/Shader.h"



namespace Starfall {

	class FillPass : public Pass {
		private:
			bool enabled;
		public:
			FillPass(bool& enabled);
			virtual void beginPass();
			virtual void endPass();
	};

	class LinePass : public Pass {
		private:
			Shader::Ptr shader;
			bool enabled;
		public:
			virtual void beginPass();
			virtual void endPass();
			LinePass(Shader::Ptr shader, bool& enabled);
	};

	class ToonTechnique : public Technique {
		private:
			ToonTechnique();
			Shader::Ptr shader;
			bool enabled; 
			//Warning: I was unabled to fix an issue with the stencil buffer on Windows 7 running inside VMWare Fusion.
			//I am making it possible to disable the stencil buffer in the config, for cses where this issue persists. 
		public:

			static Technique::Ptr Create();

			virtual void load();
			virtual void beginPasses();
			virtual void endPasses();
	};

}
