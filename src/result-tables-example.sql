CREATE TABLE road (
idroad INT ,
label TINYTEXT ,
category TINYTEXT ,
CL_BandwidthMax INT ,
CL_BandwidthMin INT ,
CL_LinesAmountMax INT ,
CL_LinesAmountMin INT ,
CL_MinimalIllumination INT ,
CL_StandartTravelSpeedMax INT ,
CL_StandartTravelSpeedMin INT ,
CL_ProvidedWithIllumination BOOL ,
CL_Geometry_CL_Coordinates TEXT ,
CL_City_idCL_City INT ,
PRIMARY KEY(idroad)
);
CREATE TABLE CL_time_ProperInterval (
idCL_time_ProperInterval INT ,
label TINYTEXT ,
CL_time_Beginning_CL_time_inXSDDateTime DATETIME ,
CL_time_Duration_CL_time_minutes INT ,
CL_time_End_CL_time_inXSDDateTime DATETIME ,
CL_City_idCL_City INT ,
PRIMARY KEY(idCL_time_ProperInterval)
);
CREATE TABLE CL_City (
idCL_City INT ,
label TINYTEXT ,
category TINYTEXT ,
CL_Name TEXT ,
CL_Geometry_CL_Coordinates TEXT ,
PRIMARY KEY(idCL_City)
);
CREATE TABLE CL_Lighting (
idCL_Lighting INT ,
label TINYTEXT ,
CL_DistanceBetween INT ,
CL_LampType TEXT ,
CL_Power INT ,
CL_PowerPerKilometer INT ,
PRIMARY KEY(idCL_Lighting)
);
CREATE TABLE CL_RoadIllumination (
idCL_RoadIllumination INT ,
label TINYTEXT ,
CL_IlluminationAmount INT ,
CL_Lighting_idCL_Lighting INT ,
road_idroad INT ,
PRIMARY KEY(idCL_RoadIllumination)
);
