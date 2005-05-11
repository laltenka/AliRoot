#ifndef ALIITSVSDD03_H
#define ALIITSVSDD03_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
/*
  $Id$
*/
// ITS step manager and geometry class for the ITS SDD test beam geometry
// of Summer 2003 and later.
// At present, the geometry and materials need to be checked against the
// proper geometry of the 2003 test beam. In addition, because the SSD
// used in the test beam are single sided, the second side needs be ignored.
// This can cause some problems with the SSD reconstruction code.

#include "AliITS.h"

typedef enum {
  kNoTarg=0,kIron=4,kLead,kSteel,
  kCarbon=8,kAl,kBe,kTi,kSn,kCopper,kGe,kTungsten=19
} TargTyp_t;

class AliITSvSDD03 : public AliITS{
 public:
    AliITSvSDD03(); // default constructor
    AliITSvSDD03(const char *title,Int_t year); // standard constructor
    AliITSvSDD03(const AliITSvSDD03 &source); // Copy constructor
    AliITSvSDD03& operator=(const AliITSvSDD03 &source); // = operator
    virtual ~AliITSvSDD03(); // destructor
    virtual void   BuildGeometry();
    virtual void   CreateGeometry();
    virtual void   CreateMaterials();
    virtual Int_t  IsVersion() const {// returns the ITS version number 
                                      return 1;} 
    virtual void   Init(); 
    virtual void   SetDefaults();
    virtual void   DrawModule() const;
    virtual void   StepManager(); 
    virtual void   SetTargMat(TargTyp_t tt=kNoTarg){ // set target material 
                                                 fTarg=tt;}    
    virtual void   SetTargThick(Float_t th=1){ // set target thickness 
                                                 fTargThick=th;}    
    virtual void   SetWriteDet(Bool_t det=kTRUE){ // set .det write
                                                 fGeomDetOut = det;}
    virtual void   SetWriteDet(const char *f){ // set write file
                                     strncpy(fWrite,f,60);fGeomDetOut = kTRUE;}
    virtual void   SetReadDet(Bool_t det=kTRUE){ //set .det read
                                                fGeomDetIn = det;}
    virtual void   SetReadDet(const char *f){ // set read file
                                       strncpy(fRead,f,60);fGeomDetIn = kTRUE;}
    virtual void   SetEUCLIDFileName(const char *f){ // set write file
                             fEuclidGeometry=f;fEuclidOut = kTRUE;}
    virtual void   SetMinorVersion(Int_t v=22){ // Choose between existing minor versions
        fMinorVersion = v;} 
    virtual void   SetThicknessDet1(Float_t v=300.){
         // Set detector thickness in layer 1
         fDet1 = v;}
    virtual void   SetThicknessDet2(Float_t v=300.){
         // Set detector thickness in layer 2
         fDet2 = v;}
    virtual void   SetThicknessChip1(Float_t v=300.){
         // Set chip thickness in layer 1
         fChip1 = v;}
    virtual void   SetThicknessChip2(Float_t v=300.){
         // Set chip thickness in layer 2
         fChip2 = v;}
    // Replacement default simulation initilization.
    virtual void SetDefaultSimulation();
    TargTyp_t GetTargMat() const {return fTarg;}
    // Decodes the id and copy nuber to give the layer, ladder, and detector 
    // numbers . Returns the module number.
    virtual Int_t DecodeDetector(Int_t id,Int_t cpy,Int_t &lay,
                                 Int_t &lad,Int_t &det) const;
         //
 private:  
    void InitAliITSgeom();
    Bool_t fGeomDetOut;       // Flag to write .det file out
    Bool_t fGeomDetIn;        // Flag to read .det file or directly from Geat.
    Int_t  fMajorVersion;     // Major version number == IsVersion
    Int_t  fMinorVersion;     // Minor version number 
    char   fEuclidGeomDet[60];// file where detector transormation are define.
    char   fRead[60];         //! file name to read .det file
    char   fWrite[60];        //! file name to write .det file 
    Float_t  fDet1;           // thickness of detector in SDD layer 1
    Float_t  fDet2;           // thickness of detector in SDD layer 2
    Float_t  fChip1;          // thickness of chip in SDD layer 1
    Float_t  fChip2;          // thickness of chip in SDD layer 2 
    Int_t fIDMother;          //! ITS Mother Volume id.
    Int_t fYear;              // Year flag to select different geometries.
    TargTyp_t fTarg;          // Target material
    Float_t fTargThick;       // TargetThickness in mm

    ClassDef(AliITSvSDD03,2) // Hits manager and geometry for SDD testbeam
};
#endif
