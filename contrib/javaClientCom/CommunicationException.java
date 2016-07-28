package org.yars.communication;

public class CommunicationException extends Exception
{
  private String _message = "";
  public CommunicationException(String message)
  {
    super(message);
    _message = message;
  }

  public String message()
  {
    return _message;
  }

}
