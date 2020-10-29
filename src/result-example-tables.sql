CREATE TABLE road (
idroad INT ,
label TINYTEXT ,
category TINYTEXT ,
CL_BandwidthMax INT ,
CL_BandwidthMin INT ,
CL_Geometry_Coordinates TEXT ,
CL_LinesAmountMax INT ,
CL_City_idCL_City INT ,
CL_LinesAmountMin INT ,
CL_MinimalIllumination INT ,
CL_StandartTravelSpeedMax INT ,
CL_StandartTravelSpeedMin INT ,
CL_ProvidedWithIllumination BOOL ,
PRIMARY KEY(idroad)
);
CREATE TABLE CL_time_ProperInterval (
idCL_time_ProperInterval INT ,
label TINYTEXT ,
CL_time_Beginning_time_inXSDDateTime DATETIME ,
CL_time_Duration_time_minutes INT ,
CL_time_End_time_inXSDDateTime DATETIME ,
CL_City_idCL_City INT ,
PRIMARY KEY(idCL_time_ProperInterval)
);
CREATE TABLE CL_City (
idCL_City INT ,
label TINYTEXT ,
category TINYTEXT ,
CL_Geometry_Coordinates TEXT ,
CL_Name TEXT ,
PRIMARY KEY(idCL_City)
);
CREATE TABLE CL_Lighting (
idCL_Lighting INT ,
label TINYTEXT ,
CL_DistanceBetween INT ,
CL_LampType TEXT ,
CL_Power TEXT ,
CL_PowerPerKilometer INT ,
PRIMARY KEY(idCL_Lighting)
);
CREATE TABLE CL_RoadIllumination (
idCL_RoadIllumination INT ,
label TINYTEXT ,
CL_IlluminationAmount INT ,
CL_Lighting_idCL_Lighting INT ,
CL_road_idCL_road INT ,
PRIMARY KEY(idCL_RoadIllumination)
);
