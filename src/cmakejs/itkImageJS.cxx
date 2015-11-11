
#include "itkImageJS.h"

itkImageJS::itkImageJS(){
  m_Interpolate = InterpolateFunctionType::New();  
}

/*
* This function should only be used when executing in NODE.js in order to mount 
* a path in the filesystem to the NODEFS system. 
*
*/
void itkImageJS::MountDirectory(const string filename){
  
  EM_ASM_({

      var path = require('path');
      var fs = require('fs');
      var allpath = Pointer_stringify($0);
      var dir = path.dirname(allpath);

      var currentdir = path.sep;
      var sepdirs = dir.split(path.sep);

      for(var i = 0; i < sepdirs.length; i++){
        currentdir += sepdirs[i];
        try{
          FS.mkdir(currentdir);
        }catch(e){
          //console.error(e);
        }
        currentdir += path.sep;
      }
      try{
        FS.mount(NODEFS, { root: currentdir }, dir);
      }catch(e){
        //console.error(e);
      }

    }, filename.c_str()
  );
  
}

/*
* This function reads an image from the NODEFS or IDBS system and sets up the different attributes in itkImageJS
* If executing in the browser, you must save the image first using FS.write(filename, buffer).
* If executing inside NODE.js use mound directory with the image filename. 
*/

  void itkImageJS::ReadImage(){

    try{
      
      ImageFileReader::Pointer reader = ImageFileReader::New();
      char* filename = (char*)this->GetFilename();
      reader->SetFileName(filename);
      reader->Update();

      this->SetImage(reader->GetOutput());
      m_Interpolate->SetInputImage(this->GetImage());

      this->Initialize();

    }catch(itk::ExceptionObject & err){
      cerr<<err<<endl;
    }
    
  }

  /*
  * After reading the image, it sets up different attributes
  */
  void itkImageJS::Initialize(){
    SizeType size = this->GetImage()->GetLargestPossibleRegion().GetSize();
    m_Size[0] = size[0];
    m_Size[1] = size[1];
    m_Size[2] = size[2];

    SpacingType spacing = this->GetImage()->GetSpacing();
    m_Spacing[0] = spacing[0];
    m_Spacing[1] = spacing[1];
    m_Spacing[2] = spacing[2];

    PointType origin = this->GetImage()->GetOrigin();
    m_Origin[0] = origin[0];
    m_Origin[1] = origin[1];
    m_Origin[2] = origin[2];

    DirectionType direction = this->GetImage()->GetDirection();
    for(int i = 0; i < dimension*dimension; i++){
      m_Direction[i] = direction[i/dimension][i%dimension];
    }
  }

  /*
  * Write the image to to the file system. 
  */
  void itkImageJS::WriteImage(){
    try{
    
      ImageFileWriter::Pointer writer = ImageFileWriter::New();
      char* filename = (char*)this->GetFilename();
      writer->SetFileName(filename);
      writer->SetInput(this->GetImage());
      writer->Update();
    }catch(itk::ExceptionObject & err){
      cerr<<err<<endl;
    }

  }