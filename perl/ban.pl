
 $Target_Directory = "D:\\SVN\\Interface_WiFi_Audio\\branches\\Interface_WiFi_Audio";
  
  
  opendir (Dir,$Target_Directory);
  	
if (! open(GCM_FILE, "+>D:\\SVN\\Interface_WiFi_Audio\\branches\\Interface_WiFi_Audio\\gcm\\test\.jdshjdsh"))
{
  print "* ** Impossible d'ouvrir le fichier $Target_Directory\\$SW_Name$SW_Version_WithoutPoint\.gcm ** \n";
  #system("pause");
  die "*\n#*************************************************************************#\n";  
}
else
{
 print "* **  ouverture : $SW_Name$SW_Version_WithoutPoint\.gcm ok** \n";
 # system("pause");
}


 