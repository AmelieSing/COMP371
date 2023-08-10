         #version 330 core

         layout (location = 0) in vec3 aPos;
         layout (location = 1) in vec3 aNormals;
         layout (location = 2) in vec2 aUV;
         
         uniform mat4 worldMatrix;
         uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
         uniform mat4 projectionMatrix = mat4(1.0);
         uniform mat4 light_view_proj_matrix;

         out vec3 fragment_normal;
         out vec3 fragment_position;
         out vec4 fragment_position_light_space;

         out vec2 vertexUV;
        
         void main()
         {
            fragment_normal = mat3(worldMatrix) * aNormals;
            fragment_position = vec3(worldMatrix * vec4(aPos, 1.0));
            fragment_position_light_space = light_view_proj_matrix * vec4(fragment_position, 1.0);

            mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
            gl_Position = modelViewProjection * vec4(aPos, 1.0);
            vertexUV = aUV;
         }