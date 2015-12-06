/* vector test (test 5) */
{ 
   vec4 temp;
   vec4 v43332 = vec4(3.0, 3.0, 3.0, 2.0);
   vec4 v45551 = vec4(5.0, 5.0, 5.0, 1.0);
   bvec4 btttf = bvec4(true, true, true, false);

   temp[0] = 0.0; /* blue */
   temp[1] = 0.0;
   temp[2] = 1.0;
   temp[3] = 1.0;

if (!(v43332[3] ^ v43332[3] ==  v45551[3] * (v45551[0] - v45551[3]))){/* !(2 ^ 2 == 1 * (5 - 1))*/

   temp[0] = 1.0; /* red */
   temp[1] = 0.0;
   temp[2] = 0.0;
   temp[3] = 1.0;

} else if(v43332[3] ^ v43332[3] ==  v45551[3] * (v45551[0] - v45551[3]) && btttf[3]){
   temp[0] = 1.0; /* red */
   temp[1] = 0.0;
   temp[2] = 0.0;
   temp[3] = 1.0;

} else if(v43332[3] ^ v43332[3] ==  v45551[3] * (v45551[0] - v45551[3]) || btttf[3]){
   temp[0] = 0.0; /* green */
   temp[1] = 1.0;
   temp[2] = 0.0;
   temp[3] = 1.0;
}

  gl_FragColor = temp;

}