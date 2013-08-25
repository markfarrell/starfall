//Copyright (c) 2013 Mark Farrell
#pragma once

#include "Starfall\Technique.h"
#include "Starfall\Shader.h"



namespace Starfall {

	class FillPass : public Pass {
		public:
			virtual void beginPass();
	};

	class LinePass : public Pass {
		private:
			Shader::Ptr shader;
		public:
			virtual void beginPass();
			virtual void endPass();
			LinePass(Shader::Ptr shader);
	};

	class ToonTechnique : public Technique {
		private:
			ToonTechnique();
			Shader::Ptr shader;
		public:

			static Technique::Ptr Create();

			virtual void load();
			virtual void beginPasses();
			virtual void endPasses();
	};

}