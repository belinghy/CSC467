/* if statement test (test 4)*/
{ 
   vec4 temp;

   bool level1 = false; 
   bool level2 = false;
   bool level22 = true;
   

   temp[0] = 0.0; /* blue */
   temp[1] = 0.0;
   temp[2] = 1.0;
   temp[3] = 1.0;

if (level1){

   temp[0] = 1.0; /* red */
   temp[1] = 0.0;
   temp[2] = 0.0;
   temp[3] = 1.0;
}else{

   if(level2){
      temp[0] = 1.0; /* yellow */
	  temp[1] = 1.0;
	  temp[2] = 0.0;
	  temp[3] = 1.0;

   } else {
      temp[0] = 0.0; /* cyan */
	  temp[1] = 1.0;
	  temp[2] = 1.0;
	  temp[3] = 1.0;
   
     if(level22){
      temp[0] = 1.0; /* magenta */
      temp[1] = 0.0;
      temp[2] = 1.0;
      temp[3] = 1.0;
     } else {
       temp[0] = 0.0; /* green */
       temp[1] = 1.0;
       temp[2] = 0.0;
       temp[3] = 1.0;
     }
   }
}

  gl_FragColor = temp;

}
