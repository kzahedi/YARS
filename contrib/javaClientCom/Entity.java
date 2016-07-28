package org.yars.communication;

import java.util.Vector;

public class Entity
{
  public String         name          = "";
  public int            dimension     = -1;
  public Vector<Domain> mappedDomains = new Vector<Domain>();
  public Vector<Domain> rawDomains    = new Vector<Domain>();
  public Vector<Double> values        = new Vector<Double>();
}
 
